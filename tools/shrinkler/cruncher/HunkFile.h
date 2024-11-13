// Copyright 1999-2022 Aske Simon Christensen. See LICENSE.txt for usage terms.

/*

Operations on Amiga executables, including loading, parsing,
hunk merging, crunching and saving.

*/

#pragma once

#include <cstring>
#include <algorithm>
#include <string>
#include <utility>
#include <algorithm>

using std::make_pair;
using std::max;
using std::min;
using std::pair;
using std::string;

#include "doshunks.h"
#include "AmigaWords.h"
#include "DecrunchHeaders.h"
#include "Pack.h"
#include "RangeDecoder.h"
#include "LZDecoder.h"
#include "Verifier.h"

const char *hunktype[HUNK_ABSRELOC16-HUNK_UNIT+1] = {
	"UNIT","NAME","CODE","DATA","BSS ","RELOC32","RELOC16","RELOC8",
	"EXT","SYMBOL","DEBUG","END","HEADER","","OVERLAY","BREAK",
	"DREL32","DREL16","DREL8","LIB","INDEX",
	"RELOC32SHORT","RELRELOC32","ABSRELOC16"
};

#define HUNKF_MASK (HUNKF_FAST | HUNKF_CHIP)
#define NUM_RELOC_CONTEXTS 256

class HunkInfo {
public:
	HunkInfo() {
		type = 0;
		relocstart = 0;
		relocshortstart = 0;
		relocentries = 0;
	}

	unsigned type;        // HUNK_<type>
	unsigned flags;       // HUNKF_<flag>
	int memsize,datasize; // longwords
	int datastart;        // longword index in file
	int relocstart;       // longword index in file
	int relocshortstart;  // longword index in file
	int relocentries;     // no. of entries
};

// Compare waste space
class HunkMergeCompare {
	vector<HunkInfo>& hunks;

	int waste(int h) {
		if (hunks[h].type == HUNK_BSS) {
			return hunks[h].memsize;
		} else {
			return hunks[h].memsize - hunks[h].datasize;
		}
	}
public:
	HunkMergeCompare(vector<HunkInfo>& hunks) : hunks(hunks) {}

	bool operator()(int h1, int h2) {
		return waste(h1) < waste(h2);
	}
};


class HunkFile {
	vector<Longword> data;
	vector<HunkInfo> hunks;
	int relocshort_total_size;

	vector<unsigned char> compress_hunks(PackParams *params, bool overlap, bool mini, RefEdgeFactory *edge_factory, bool show_progress) {
		int numhunks = hunks.size();

		vector<unsigned char> pack_buffer;
		RangeCoder range_coder(LZEncoder::NUM_CONTEXTS + NUM_RELOC_CONTEXTS, pack_buffer);

		// Print compression status header
		const char *ordinals[] = { "st", "nd", "rd", "th" };
		printf("Hunk  Original");
		for (int p = 1 ; p <= params->iterations ; p++) {
			printf("  After %d%s pass", p, ordinals[min(p,4)-1]);
		}
		if (!mini) {
			printf("      Relocs");
		}
		printf("\n");

		// Crunch the hunks, one by one.
		for (int h = 0 ; h < (mini ? 1 : numhunks) ; h++) {
			printf("%4d  ", h);
			range_coder.reset();
			switch (hunks[h].type) {
			case HUNK_CODE:
			case HUNK_DATA:
				{
					// Pack data
					unsigned char *hunk_data = (unsigned char *) &data[hunks[h].datastart];
					int hunk_data_length = hunks[h].datasize * 4;
					// Trim trailing zeros
					while (hunk_data_length > 0 && hunk_data[hunk_data_length - 1] == 0) {
						hunk_data_length--;
					}
					int zero_padding = mini ? 0 : hunks[h].memsize * 4 - hunk_data_length;
					packData(hunk_data, hunk_data_length, zero_padding, params, &range_coder, edge_factory, show_progress);
				}
				break;
			default:
				int zero_padding = mini ? 0 : hunks[h].memsize * 4;
				packData(NULL, 0, zero_padding, params, &range_coder, edge_factory, show_progress);
				break;
			}

			if (!mini) {
				// Reloc table
				int reloc_size = 0;
				for (int rh = 0 ; rh < numhunks ; rh++) {
					vector<int> offsets;
					if (hunks[h].relocstart != 0) {
						int spos = hunks[h].relocstart;
						while (data[spos] != 0) {
							int rn = data[spos++];
							if (data[spos++] == rh) {
								while (rn--) {
									offsets.push_back(data[spos++]);
								}
							} else {
								spos += rn;
							}
						}
						sort(offsets.begin(), offsets.end());
					}
					int last_offset = -4;
					for (int ri = 0 ; ri < offsets.size() ; ri++) {
						int offset = offsets[ri];
						int delta = offset - last_offset;
						if (delta < 4) {
							printf("\n\nError in input file: overlapping reloc entries.\n\n");
							exit(1);
						}
						reloc_size += range_coder.encodeNumber(LZEncoder::NUM_CONTEXTS, delta);
						last_offset = offset;
					}
					reloc_size += range_coder.encodeNumber(LZEncoder::NUM_CONTEXTS, 2);
				}
				printf("  %10.3f", reloc_size / (double) (8 << Coder::BIT_PRECISION));
			}
			printf("\n");
			fflush(stdout);
		}
		range_coder.finish();
		printf("\n");

		// Round up compressed size to a whole number of longwords
		pack_buffer.resize((pack_buffer.size() + 3) & -4, 0);
		return pack_buffer;
	}

	vector<pair<int,int> > verify(vector<unsigned char>& pack_buffer, bool overlap, bool mini) {
		int numhunks = hunks.size();
		vector<pair<int,int> > count_and_hunksize;

		printf("Verifying... ");
		fflush(stdout);
		RangeDecoder decoder(LZEncoder::NUM_CONTEXTS + NUM_RELOC_CONTEXTS, pack_buffer);
		LZDecoder lzd(&decoder, true);
		for (int h = 0 ; h < (mini ? 1 : numhunks) ; h++) {
			unsigned char *hunk_data;
			int hunk_data_length = hunks[h].datasize * 4;
			if (hunks[h].type != HUNK_BSS) {
				// Find hunk data
				hunk_data = (unsigned char *) &data[hunks[h].datastart];
				if (mini) {
					// Trim trailing zeros
					while (hunk_data_length > 0 && hunk_data[hunk_data_length - 1] == 0) {
						hunk_data_length--;
					}
				}
			} else {
				// Signal empty hunk by NULL data pointer
				hunk_data = NULL;
			}

			// Verify data
			bool error = false;
			LZVerifier verifier(h, hunk_data, hunk_data_length, hunks[h].memsize * sizeof(Longword), sizeof(Longword));
			decoder.reset();
			decoder.setListener(&verifier);
			if (!lzd.decode(verifier)) {
				error = true;
			}

			// Check length
			if (!error && !mini && verifier.size() != hunks[h].memsize * sizeof(Longword)) {
				printf("Verify error: hunk %d has incorrect length (%d, should have been %d)!\n", h, verifier.size(), hunk_data_length);
				error = true;
			}

			if (error) {
				internal_error();
			}

			if (!mini) {
				// Skip relocs
				for (int rh = 0 ; rh < numhunks ; rh++) {
					int delta;
					do {
						delta = decoder.decodeNumber(LZEncoder::NUM_CONTEXTS);
					} while (delta != 2);
				}
			}

			int margin = verifier.front_overlap_margin;
			int count = verifier.compressed_read_count;
			int min_hunksize = (margin == 0 ? 1 : (margin + 3) / 4) + count;
			count_and_hunksize.push_back(make_pair(count, min_hunksize));
		}
		printf("OK\n\n");

		return count_and_hunksize;
	}

public:
	void load(const char *filename) {
		FILE *file;
		if ((file = fopen(filename, "rb"))) {
			fseek(file, 0, SEEK_END);
			int length = ftell(file);
			fseek(file, 0, SEEK_SET);
			if (length & 3) {
				printf("File %s has an illegal size!\n\n", filename);
				fclose(file);
				exit(1);
			}
			data.resize(length / 4);
			if (fread(&data[0], 4, data.size(), file) == data.size()) {
				fclose(file);
				return;
			}
		}

		printf("Error while reading file %s\n\n", filename);
		exit(1);
	}

	void save(const char *filename) {
		FILE *file;
		if ((file = fopen(filename, "wb"))) {
			if (fwrite(&data[0], 4, data.size(), file) == data.size()) {
				fclose(file);
				return;
			}
		}

		printf("Error while writing file %s\n\n", filename);
		exit(1);
	}

	int size() {
		return data.size() * 4;		
	}

	bool analyze() {
		int index = 0;
		int length = data.size();

		if (data[index++] != HUNK_HEADER) {
			printf("No hunk header!\n");
			return false;
		}

		while (data[index++]) {
			index += data[index];
			if (index >= length) {
				printf("Bad hunk header!\n");
				return false;
			}
		}

		int numhunks = data[index++];
		if (numhunks == 0) {
			printf("No hunks!\n");
			return false;
		}

		if (data[index++] != 0 || data[index++] != numhunks-1) {
			printf("Unsupported hunk load limits!\n");
			return false;
		}

		hunks.clear();
		hunks.resize(numhunks);
		for (int h = 0 ; h < numhunks ; h++) {
			hunks[h].memsize = data[index] & 0x0fffffff;
			switch (hunks[h].flags = data[index] & 0xf0000000) {
			case 0:
			case HUNKF_CHIP:
			case HUNKF_FAST:
				break;
			default:
				printf("Illegal hunk flags!\n");
				return false;
			}
			index++;
		}

		relocshort_total_size = 0;

		// Parse hunks
		printf("Hunk  Mem  Type  Mem size  Data size  Data sum  Relocs\n");
		for (int h = 0, nh = 0 ; h < numhunks ;) {
			unsigned flags = hunks[h].flags, type;
			int hunk_length, symlen, n_symbols;
			int lh = h;
			bool missing_relocs = false;
			const char *note = "";
			while (lh == h) {
				if (index >= length) {
					printf("\nUnexpected end of file!\n");
					return false;
				}
				type = data[index++] & 0x0fffffff;
				if (index >= length && type != HUNK_END) {
					printf("\nUnexpected end of file!\n");
					return false;
				}

				if (missing_relocs && type != HUNK_RELOC32 && type != HUNK_RELOC32SHORT && type != HUNK_DREL32) {
					if (hunks[h].relocentries > 0) {
						printf("  %6d%s\n", hunks[h].relocentries, note);
					} else {
						printf("        %s\n", note);
					}
					note = "";
					missing_relocs = false;
				}
				switch (type) {
				case HUNK_UNIT:
				case HUNK_NAME:
				case HUNK_DEBUG:
					printf("           %s (skipped)\n",hunktype[type-HUNK_UNIT]);
					hunk_length = data[index++];
					index += hunk_length;
					break;
				case HUNK_SYMBOL:
					n_symbols = 0;
					symlen = data[index++];
					while (symlen > 0) {
						n_symbols++;
						index += symlen+1;
						symlen = data[index++];
					}
					printf("           SYMBOL (%d entries)\n", n_symbols);
					break;
				case HUNK_CODE:
				case HUNK_DATA:
				case HUNK_BSS:
					if (nh > h) {
						h = nh;
						index--;
						break;
					}
					hunks[h].type = type;
					hunks[h].datasize = data[index++];
					printf("%4d  %s %4s%10d %10d",
						h, flags == HUNKF_CHIP ? "CHIP" : flags == HUNKF_FAST ? "FAST" : "ANY ",
						hunktype[type-HUNK_UNIT], hunks[h].memsize*4, hunks[h].datasize*4);
					if (type != HUNK_BSS) {
						hunks[h].datastart = index;
						index += hunks[h].datasize;
						if (hunks[h].datasize > 0) {
							int sum = 0;
							for (int pos = hunks[h].datastart ; pos < hunks[h].datastart+hunks[h].datasize ; pos++) {
								sum += data[pos];
							}
							printf("  %08x", sum);
						} else {
							printf("          ");
						}
					}
					if (hunks[h].datasize > hunks[h].memsize) {
						note = "  Hunk size overflow corrected!";
						hunks[h].memsize = hunks[h].datasize;
					}
					nh = h+1;
					missing_relocs = true;
					break;
				case HUNK_RELOC32:
					if (hunks[h].relocstart != 0) {
						printf("\nMultiple reloc tables!\n");
						return false;
					}
					hunks[h].relocstart = index;
					{
						int n;
						while ((n = data[index++]) != 0) {
							if (n < 0 || index+n+2 >= length || data[index++] >= numhunks) {
								printf("\nError in reloc table!\n");
								return false;
							}
							hunks[h].relocentries += n;
							while (n--) {
								if (data[index++] > hunks[h].memsize*4-4) {
									printf("\nError in reloc table!\n");
									return false;
								}
							}
						}
						note = "";
					}
					break;
				case HUNK_RELOC32SHORT:
				case HUNK_DREL32:
					if (hunks[h].relocshortstart != 0) {
						printf("\nMultiple reloc tables!\n");
						return false;
					}
					hunks[h].relocshortstart = index;
					{
						Word *word_data = (Word *) &data[index];
						int word_index = 0;
						int n;
						while ((n = word_data[word_index++]) != 0) {
							if (n < 0 || index + (word_index+n+2)/2+1 >= length || word_data[word_index++] >= numhunks) {
								printf("\nError in reloc table!\n");
								return false;
							}
							hunks[h].relocentries += n;
							while (n--) {
								if (word_data[word_index++] > hunks[h].memsize*4-4) {
									printf("\nError in reloc table!\n");
									return false;
								}
							}
						}
						int size_in_longwords = (word_index+1)/2;
						index += size_in_longwords;
						relocshort_total_size += size_in_longwords;
					}
					break;
				case HUNK_END:
					if (hunks[h].type == 0) {
						printf("Empty%9d\n", hunks[h].memsize*4);
						return false;
					}
					h = h+1; nh = h;
					break;
				case HUNK_RELOC16:
				case HUNK_RELOC8:
				case HUNK_EXT:
				case HUNK_HEADER:
				case HUNK_OVERLAY:
				case HUNK_BREAK:
				case HUNK_DREL16:
				case HUNK_DREL8:
				case HUNK_LIB:
				case HUNK_INDEX:
				case HUNK_RELRELOC32:
				case HUNK_ABSRELOC16:
					printf("           %s (unsupported)\n",hunktype[type-HUNK_UNIT]);
					return false;
				default:
					printf("           Unknown (%08X)\n",type);
					return false;
				}
			}
		}

		if (index < length) {
			printf("Warning: %d bytes of extra data at the end of the file!\n", (length-index)*4);
		}
		printf("\n");
		return true;
	}

	bool requires_hunk_processing() {
		return relocshort_total_size != 0;
	}

	int memory_usage(bool include_last_hunk) {
		int sum = 0;
		int hunks_to_sum = include_last_hunk ? hunks.size() : hunks.size() - 1;
		for (int h = 0 ; h < hunks_to_sum ; h++) {
			sum += ((hunks[h].memsize * 4 + 4) & -8) + 8;
		}
		return sum;
	}

	vector<pair<unsigned, vector<int> > > identity_hunklist() {
		int numhunks = hunks.size();
		vector<pair<unsigned, vector<int> > > hunklist;
		for (int sh = 0 ; sh < numhunks ; sh++) {
			hunklist.push_back(make_pair(hunks[sh].type | hunks[sh].flags, vector<int>()));
			hunklist.back().second.push_back(sh);
		}
		return hunklist;
	}

	vector<pair<unsigned, vector<int> > > merged_hunklist() {
		int numhunks = hunks.size();
		vector<pair<unsigned, vector<int> > > hunklist(3);
		unsigned flags0 = hunks[0].flags;
		unsigned flags1 = (~flags0) & HUNKF_CHIP;
		unsigned flags2 = HUNKF_CHIP + HUNKF_FAST - flags0 - flags1;
		hunklist[0].first = flags0 | HUNK_CODE;
		hunklist[1].first = flags1 | HUNK_CODE;
		hunklist[2].first = flags2 | HUNK_CODE;

		HunkMergeCompare comp(hunks);

		// Go through the 3 resulting hunks, one for each memory type.
		for (int dh = 0 ; dh < 3 ; dh++) {
			for (int sh = 0 ; sh < numhunks ; sh++) {
				if (hunks[sh].flags == (hunklist[dh].first & HUNKF_MASK)) {
					hunklist[dh].second.push_back(sh);
				}
			}
			stable_sort(hunklist[dh].second.begin(), hunklist[dh].second.end(), comp);
		}

		// Remove unused memory types
		vector<pair<unsigned, vector<int> > > result;
		for (int dh = 0 ; dh < 3 ; dh++) {
			if (hunklist[dh].second.size() > 0) {
				result.push_back(hunklist[dh]);
			}
		}

		return result;
	}

	HunkFile* merge_hunks(const vector<pair<unsigned, vector<int> > >& hunklist) {
		int numhunks = hunks.size();
		int dnh = hunklist.size();
		int bufsize = data.size()+relocshort_total_size+3; // Reloc can write 3 further temporarily.

		// Calculate safe size of new file buffer
		for (int dh = 0 ; dh < dnh ; dh++) {
			int waste = 0;
			for (int shi = 0 ; shi < hunklist[dh].second.size() ; shi++) {
				int sh = hunklist[dh].second[shi];
				if (hunks[sh].type != HUNK_BSS) {
					bufsize += waste;
					waste = -hunks[sh].datasize;
				}
				waste += hunks[sh].memsize;
			}
		}

		// Processed file
		HunkFile *ef = new HunkFile;
		ef->data.resize(bufsize, 0);
		ef->hunks.resize(dnh);

		vector<int> dhunk(numhunks);
		vector<int> offset(numhunks);

		// Find destination hunk and offset for all source hunks.
		for (int dh = 0 ; dh < dnh ; dh++) {
			unsigned hunkf = hunklist[dh].first;
			ef->hunks[dh].type  = hunkf & 0x0fffffff;
			ef->hunks[dh].flags = hunkf & 0xf0000000;
			int memsize = 0;
			int datasize = 0;
			for (int shi = 0 ; shi < hunklist[dh].second.size() ; shi++) {
				int sh = hunklist[dh].second[shi];
				memsize = (memsize+1)&-2;
				dhunk[sh] = dh;
				offset[sh] = memsize*4;
				if (hunks[sh].type != HUNK_BSS) {
					datasize = memsize + hunks[sh].datasize;
				}
				memsize += hunks[sh].memsize;
			}
			ef->hunks[dh].memsize = memsize;
			ef->hunks[dh].datasize = datasize;
		}

		// Write new hunk header
		int dpos = 0;
		ef->data[dpos++] = HUNK_HEADER;
		ef->data[dpos++] = 0;
		ef->data[dpos++] = ef->hunks.size();
		ef->data[dpos++] = 0;
		ef->data[dpos++] = ef->hunks.size()-1;
		for (int dh = 0 ; dh < ef->hunks.size() ; dh++) {
			ef->data[dpos++] = ef->hunks[dh].memsize | ef->hunks[dh].flags;
		}

		// Generate new hunks
		for (int dh = 0 ; dh < dnh ; dh++) {
			// Put hunk type and data (or bss) size.
			ef->data[dpos++] = ef->hunks[dh].type;
			ef->data[dpos++] = ef->hunks[dh].datasize;
			ef->hunks[dh].datastart = dpos;

			// Run through the implied source hunks.
			int hoffset = 0;
			for (int shi = 0 ; shi < hunklist[dh].second.size() ; shi++) {
				int sh = hunklist[dh].second[shi];
				if (hunks[sh].type != HUNK_BSS) {
					// Fill the gap.
					for(; hoffset < offset[sh] ; hoffset += 4) {
						ef->data[dpos++] = 0;
					}
					// Copy the data.
					for (int spos = hunks[sh].datastart ; spos < hunks[sh].datastart + hunks[sh].datasize ; spos++) {
						ef->data[dpos++] = data[spos];
					}
					hoffset += hunks[sh].datasize*4;
				}
			}

			// Transfer all reloc information to the new hunk.
			ef->data[dpos++] = HUNK_RELOC32;
			ef->hunks[dh].relocstart = dpos;
			ef->hunks[dh].relocentries = 0;
			unsigned char *bytes = (unsigned char *)&ef->data[ef->hunks[dh].datastart];
			// Iterate through destination reloc target hunk
			for (int drh = 0 ; drh < ef->hunks.size() ; drh++) {
				// Make space for number of relocs and store index of target hunk.
				int rnpos = dpos++; // Position for number of relocs
				ef->data[dpos++] = drh;
				// Transfer all appropriate reloc entries.
				int rtot = 0; // Total number of relocs in hunk
				for (int sh = 0 ; sh < numhunks ; sh++) {
					if (dhunk[sh] == dh && hunks[sh].relocstart != 0) {
						int spos = hunks[sh].relocstart;
						int rn; // Number of relocs
						while ((rn = data[spos++]) > 0) {
							int srh = data[spos++]; // Source reloc target hunk
							if (dhunk[srh] == drh) {
								rtot += rn;
								for (int ri = 0 ; ri < rn ; ri++) {
									int rv = data[spos++]; // Reloc value
									ef->data[dpos++] = rv+offset[sh];
									*((Longword *)&bytes[rv+offset[sh]]) += offset[srh];
								}
							} else {
								spos += rn;
							}
						}
					}
					if (dhunk[sh] == dh && hunks[sh].relocshortstart != 0) {
						Word *word_data = (Word *) &data[hunks[sh].relocshortstart];
						int word_spos = 0;
						int rn; // Number of relocs
						while ((rn = word_data[word_spos++]) > 0) {
							int srh = word_data[word_spos++]; // Source reloc target hunk
							if (dhunk[srh] == drh) {
								rtot += rn;
								for (int ri = 0 ; ri < rn ; ri++) {
									int rv = word_data[word_spos++]; // Reloc value
									ef->data[dpos++] = rv+offset[sh];
									*((Longword *)&bytes[rv+offset[sh]]) += offset[srh];
								}
							} else {
								word_spos += rn;
							}
						}
					}
				}
				// Store total number of relocs with the actual target hunk.
				// If there are none, remove the spaces for
				// number of relocs and target hunk.
				if (rtot == 0) {
					dpos -= 2;
				} else {
					ef->data[rnpos] = rtot;
					ef->hunks[dh].relocentries += rtot;
				}
			}
			// End the reloc section.
			// If there are no relocs, remove the reloc header.
			if (ef->hunks[dh].relocentries == 0) {
				ef->hunks[dh].relocstart = 0;
				dpos -= 1;
			} else {
				ef->data[dpos++] = 0;
			}
		}
		// There must be a HUNK_END after last hunk!
		ef->data[dpos++] = HUNK_END;
		// Note resulting file size
		ef->data.resize(dpos);

		return ef;
	}

	bool valid_mini() {
		if (!(hunks[0].type == HUNK_CODE && hunks[0].relocentries == 0)) return false;
		for (int h = 1 ; h < hunks.size() ; h++) {
			if (hunks[h].relocentries != 0) return false;
			if (hunks[h].type == HUNK_BSS || hunks[h].datasize == 0) continue;
			for (int i = 0 ; i < hunks[h].datasize ; i++) {
				if (data[hunks[h].datastart + i] != 0) return false;
			}
		}
		return true;
	}

	HunkFile* crunch(PackParams *params, bool overlap, bool mini, bool commandline, string *decrunch_text, unsigned flash_address, RefEdgeFactory *edge_factory, bool show_progress) {
		int numhunks = hunks.size();

		// Pad empty hunks
		bool forced = false;
		for (int h = 0 ; h < numhunks ; h++) {
			if (hunks[h].memsize == 0) {
				hunks[h].memsize = 1;
				printf("Mem size of hunk %d forced to 4.\n", h);
				forced = true;
			}
		}
		if (forced) {
			printf("\n");
		}

		vector<unsigned char> pack_buffer = compress_hunks(params, overlap, mini, edge_factory, show_progress);
		vector<pair<int,int> > count_and_hunksize = verify(pack_buffer, overlap, mini);

		int newnumhunks = numhunks+1;
		int bufsize = data.size() * 11 / 10 + 1000;

		HunkFile *ef = new HunkFile;
		ef->data.resize(bufsize, 0);

#define WRITE_HEADER(header) do {                                             \
	ppos = dpos;                                                              \
	memcpy(&ef->data[dpos], header, sizeof(header));                          \
	dpos += sizeof(header) / sizeof(Longword);                                \
} while (false)

#define WRITE_TEXT() do {                                                     \
	memcpy(&ef->data[dpos], decrunch_text->c_str(), decrunch_text->length()); \
	dpos += (decrunch_text->length() + 3) / sizeof(Longword);                 \
} while (false)

		int dpos = 0;

		// Write new hunk header
		ef->data[dpos++] = HUNK_HEADER;
		ef->data[dpos++] = 0;
		ef->data[dpos++] = newnumhunks;
		ef->data[dpos++] = 0;
		ef->data[dpos++] = newnumhunks-1;

		int lpos1, lpos2, ppos;
		Word *offsetp = NULL;
		if (overlap) {
			// Write hunk memory sizes
			lpos1 = dpos++;
			for (int h = 0 ; h < numhunks ; h++) {
				int hunksize = max(hunks[h].memsize, count_and_hunksize[h].second);
				ef->data[dpos++] = hunksize | hunks[h].flags;
			}

			// Write header
			ef->data[dpos++] = HUNK_CODE;
			lpos2 = dpos++;
			if (decrunch_text) {
				int tpos;
				if (commandline) {
					WRITE_HEADER(OverlapHeaderCT);
					tpos = ppos + 4;
				} else {
					WRITE_HEADER(OverlapHeaderT);
					tpos = ppos + 3;
				}
				ef->data[tpos + 6] = decrunch_text->length(); // Immediate for length of text
				offsetp = (Word *) &ef->data[tpos + 5]; // PC offset to text
			} else {
				if (commandline) {
					WRITE_HEADER(OverlapHeaderC);
				} else {
					WRITE_HEADER(OverlapHeader);
				}
			}
		} else if (mini) {
			// Write hunk memory sizes
			lpos1 = dpos++;
			for (int h = 0 ; h < numhunks ; h++) {
				ef->data[dpos++] = hunks[h].memsize | hunks[h].flags;
			}

			// Write header
			ef->data[dpos++] = HUNK_CODE;
			lpos2 = dpos++;
			if (commandline) {
				WRITE_HEADER(MiniHeaderC);
			} else {
				WRITE_HEADER(MiniHeader);
			}
			offsetp = (Word *) (((unsigned char *) &ef->data[ppos]) + 14); // PC offset to end of data
		} else {
			static_assert(sizeof(Header1) == sizeof(Header1C), "Header1 vs Header1C size mismatch");
			static_assert(sizeof(Header1T) == sizeof(Header1CT), "Header1T vs Header1CT size mismatch");
			int header1_size = sizeof(Header1) / sizeof(Longword);
			if (decrunch_text) {
				header1_size = (sizeof(Header1T) + (decrunch_text->length() + 3)) / sizeof(Longword);
			}
			for (int h = 0 ; h < numhunks ; h++) {
				int memsize = hunks[h].memsize;
				if (h == 0 && memsize < header1_size) {
					// Make space for header trampoline code
					memsize = header1_size;
				}
				ef->data[dpos++] = memsize | hunks[h].flags;
			}
			lpos1 = dpos++;

			// Write header 1
			ef->data[dpos++] = HUNK_CODE;
			ef->data[dpos++] = header1_size;
			if (decrunch_text) {
				if (commandline) {
					WRITE_HEADER(Header1CT);
				} else {
					WRITE_HEADER(Header1T);
				}
				ef->data[ppos + 3] = decrunch_text->length(); // Immediate for length of text
				WRITE_TEXT();
			} else {
				if (commandline) {
					WRITE_HEADER(Header1C);
				} else {
					WRITE_HEADER(Header1);
				}
			}

			// Write hunks
			for (int h = 1 ; h < numhunks ; h++) {
				ef->data[dpos++] = hunks[h].type;
				switch (hunks[h].type) {
				case HUNK_CODE:
				case HUNK_DATA:
					ef->data[dpos++] = 0;
					break;
				case HUNK_BSS:
					ef->data[dpos++] = hunks[h].datasize;
					break;
				}
			}

			// Write header 2
			ef->data[dpos++] = HUNK_CODE;
			lpos2 = dpos++;
			if (commandline) {
				WRITE_HEADER(Header2C);
			} else {
				WRITE_HEADER(Header2);
			}
		}

		if (flash_address) {
			// Insert flashing code
			dpos += 1;
			for (int fpos = dpos - 1 ; fpos >= dpos - 9 ; fpos--) {
				ef->data[fpos] = ef->data[fpos - 1];
			}
			Word* insts = (Word *) &ef->data[dpos - 11]; // TST instruction for interval size
			insts[0] = 0x33C3; // move.w d3,flash_address
			*(Longword *)&insts[1] = flash_address;
			insts[3] = 0x6AEC; // bpl.b readbit
			if (offsetp) *offsetp += 4;
		}

		if (overlap) {
			// Write decrunch text
			if (decrunch_text) {
				WRITE_TEXT();
			}

			// Set hunk sizes
			ef->data[lpos1] = dpos-ppos;
			ef->data[lpos2] = dpos-ppos;

			// Write hunks
			int packed_index = 0;
			for (int h = 0 ; h < numhunks ; h++) {
				ef->data[dpos++] = HUNK_DATA;
				int longwords_in_hunk = min<int>(count_and_hunksize[h].first, pack_buffer.size() / 4 - packed_index);
				ef->data[dpos++] = longwords_in_hunk + 1;
				ef->data[dpos++] = count_and_hunksize[h].first * 4;
				for (int i = 0 ; i < longwords_in_hunk ; i++) {
					ef->data[dpos++] = *(Longword*)&pack_buffer[(packed_index++) * 4];
				}
			}
		} else if (mini) {
			// Write compressed data backwards
			for (int i = pack_buffer.size() / 4 - 1 ; i >= 0 ; i--) {
				ef->data[dpos++] = *(Longword*)&pack_buffer[i * 4];
			}

			// Set hunk sizes
			ef->data[lpos1] = dpos-ppos + 32768/8*2/4; // Space for context state
			ef->data[lpos2] = dpos-ppos;

			// Write hunks
			for (int h = 0 ; h < numhunks ; h++) {
				ef->data[dpos++] = HUNK_BSS;
				ef->data[dpos++] = hunks[h].memsize;
			}

			// Set size of data in header
			int offset = (int) *offsetp + pack_buffer.size();
			if (offset > 32767) {
				printf("Size overflow: final size in mini mode must be less than 24k.\n\n");
				exit(1);
			}
			*offsetp = offset;
		} else {
			// Write compressed data backwards
			for (int i = pack_buffer.size() / 4 - 1 ; i >= 0 ; i--) {
				ef->data[dpos++] = *(Longword*)&pack_buffer[i * 4];
			}

			// Set hunk sizes
			ef->data[lpos1] = dpos-ppos;
			ef->data[lpos2] = dpos-ppos;
		}

		// There must be a HUNK_END after last hunk!
		ef->data[dpos++] = HUNK_END;
		// Note resulting file size
		ef->data.resize(dpos);

		return ef;
	}
};

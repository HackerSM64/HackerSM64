# Compress binary file
$(BUILD_DIR)/%.szp: $(BUILD_DIR)/%.bin
	$(call print,Compressing:,$<,$@)
	$(V)$(RNCPACK) -p -d -w $< $@ > /dev/null

# convert binary szp to object file
$(BUILD_DIR)/%.szp.o: $(BUILD_DIR)/%.szp
	$(call print,Converting Shrinkler to ELF:,$<,$@)
	$(V)$(LD) -r -b binary $< -o $@

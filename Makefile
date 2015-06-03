all:
	@echo "-------------------------------------------------------------"
	@echo "  You must be running OSX with Arduino installed in the"
	@echo "  default location to compile the firmware";
	@echo "-------------------------------------------------------------"
	/Applications/Arduino.app/Contents/MacOS/JavaApplicationStub --verify ~+/bnoAndLsm.ino

upload:
	@echo "-------------------------------------------------------------"
	@echo "  You must be running OSX with Arduino installed in the"
	@echo "  default location to upload the firmware to the board.";
	@echo "  preferences.txt will be used for upload as per usual.";
	@echo "-------------------------------------------------------------"
	/Applications/Arduino.app/Contents/MacOS/JavaApplicationStub --verify ~+/bnoAndLsm.ino

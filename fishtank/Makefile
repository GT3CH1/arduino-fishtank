TARGET_SERVER = web.peasenet.com
TARGET_USER = gcpease
TARGET_DIR = /opt/ota/arduino
DEVICE ?= fishtank
BOARD ?= nano_33_iot
VERSION ?= 0

install:
	scp $(DEVICE).ino.$(BOARD).bin $(TARGET_USER)@$(TARGET_SERVER):$(TARGET_DIR)/update-$(DEVICE)-$(VERSION).bin

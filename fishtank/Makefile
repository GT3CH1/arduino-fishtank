TARGET_SERVER = web.peasenet.com
TARGET_USER = gcpease
TARGET_DIR = /opt/ota/arduino
DEVICE ?= fishtank

VERSION ?= 0

install:
	scp $(DEVICE).bin $(TARGET_USER)@$(TARGET_SERVER):$(TARGET_DIR)/update-$(DEVICE)-$(VERSION).bin

#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

# env var should be loaded ('get_idf') prior to executing any cmds

PROJECT_NAME := sd_card

include $(IDF_PATH)/make/project.mk

build:
	idf.py build

flash:
	idf.py -p PORT flash

monitor:
	idf.py -p /dev/ttyUSB0 monitor

conf:
	idf.py set-target esp32; \
	idf.py menuconfig;

push:
	git pull; \                                                                 
	git add .; \                                                                
	read -p "Commit message: " MSG; \                                           
	git commit -m "$$MSG"; \                                                    
	git push origin master; 

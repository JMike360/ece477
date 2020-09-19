#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

# env var should be loaded ('get_idf') prior to executing any cmds

PROJECT_NAME := sd_card

include $(IDF_PATH)/make/project.mk

sd-build:
	idf.py build;

sd-conf:
	idf.py set-target esp32; \
	idf.py menuconfig;

# replace '/dev/ttyUSB0' with ESP32 port name

sd-run: 
	idf.py build; \
	idf.py -p /dev/ttyUSB0 flash monitor;

sd-clean:
	idf.py fullclean; \
	rm -r build;

sd-push:
	git pull; \                                                                 
	git add .; \                                                                
	read -p "Commit message: " MSG; \                                           
	git commit -m "$$MSG"; \                                                    
	git push origin master; 

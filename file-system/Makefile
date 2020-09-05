#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := sd_card

include $(IDF_PATH)/make/project.mk

push:
	git pull; \                                                                 
	git add .; \                                                                
	read -p "Commit message: " MSG; \                                           
	git commit -m "$$MSG"; \                                                    
	git push origin master; 

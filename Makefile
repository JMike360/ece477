push:
	git pull; \
	git add .; \
	read -p "Commit message: " MSG; \
	git commit -m "$$MSG"; \
	git push;

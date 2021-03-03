help:
        @echo "Working on $(SUBJECT)"
        @grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
        | grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' makefile|sed -n 's/^\(.*\): \(.*\)##\(.*\)/\1 --> \3/p'
init: ## Create default folder structure
        @echo "Initializing..."
        @cd .. && mkdir -p {$(SUBJECT)/exams,$(SUBJECT)/exercise,$(SUBJECT)/lessons,$(SUBJECT)/notes,$(SUBJECT)/search,$(SUBJECT)/src}

extract: ## Extract lessons from downloaded zips
        cd ../$(SUBJECT)/lessons
        for $f in ./*.zip; do unzip -q $f; done
        if [ $? -ne 0 ] then
                echo "Errore! estrazione file zip"
        fi
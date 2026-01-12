#!/bin/bash
#version 1.0

replace_checs_macros()
{
	module_calls=$(awk '/^\s*\w+import/' src/setup.c) #create string of all module import calls, ignore import if commented out with //, multiline comments are ignored
	modules=$(echo "$module_calls" | sed 's/_.*//') #remove everything but module name
	modules=$(echo "$modules" | awk -v RS="[ \n]+" '!n[$0]++') #remove duplicates
	modules=$(echo "$modules" | sed 's/\</modules\//g') #add path suffix to each module
	module_files=$(find $modules -name '*.c') #find all .c files inside modules
	module_calls=$(echo "$module_calls" | sed 's/);/void)/g')
	files=$(find modules src -name '*.c') #get all c files that could use templates
	module_files_count=$(echo "$module_files" | wc -w) #count number of files inside used modules
	module_files_count=$((module_files_count+1)) #because templates can also be registered inside setup.c, which doesnt get added to the list automatically
	awk -v funcs="$module_calls" -v module_files_count="$module_files_count" ' 
	FNR==1{FNUM++}
	BEGIN {
		if (FNUM<=module_files_count) {
		    split(funcs,tmp)
		    for (i in tmp) {
		        fnames[tmp[i]]
		    }	
		}
	}
	FNUM<=module_files_count { 
		if ($0 ~ /^\s*void\s*\w+\(void)/) {
	    	inFunc = ($NF in fnames ? 1 : 0)
		}
	}
	FNUM<=module_files_count {
		if (inFunc) {
		    tail = $0
		    while ( match(tail,/<[^0-9<>.-]+>/) ) {
		        tgt = substr(tail,RSTART+1,RLENGTH-2)
		       	switch (tgt){
					case /C:/:
			    		if (!(tgt in commands))                    
					    	commands[tgt] = ++commandCount  
					    break 
					default:
						if (!(tgt in components)) {
				      		components[tgt] = ++componentCount                   
						}
				}   
		        tail = substr(tail,RSTART+RLENGTH-1)
		    }
		}
	}
	FNUM>module_files_count {
		head = ""
	    tail = $0
	    while ( match(tail,/<[^0-9<>.-]+>/) ) {
	        tgt = substr(tail,RSTART+1,RLENGTH-2)
	        tgt_err = tgt 

		       	switch (tgt){
					case /C:/:                  
					    tgt = commands[tgt]
					    break
					case /T:/:                  
						if (!(tgt in tags))     
					    	tags[tgt] = ++tagCount
						tgt = tags[tgt]
						break
					default:
				      	tgt = components[tgt]
				}

			if (tgt=="") {
				tgt = "NULL_TEMPLATE"
			}
	        head = head substr(tail,1,RSTART-1) tgt
	        tail = substr(tail,RSTART+RLENGTH)
	    }
	}
	FNUM>module_files_count {
	   print head tail > (FILENAME ".c")
	}
	' $module_files src/setup.c $files
}


if [ -z "$1" ] 
	then
		echo "Not enough arguments given"
		exit
fi

if [ "$1" = "install" ]
then
	sudo cp $0 /usr/bin/che #only used for developement, in end user version file wont copy itself back
	sudo chmod 777 /usr/bin/che
elif [ "$1" = "compile" ]
then
	find . -name \*.c.* -type f -delete #in case files got not deleted because operation was cancelled
	replace_checs_macros
	make run
	find . -name \*.c.c -type f -delete
	find . -name \*.c.o -type f -delete
elif [ "$1" = "compile-dbg" ]
then
	find . -name \*.c.* -type f -delete #in case files got not deleted because operation was cancelled
	replace_checs_macros
	make run
else
	echo "Unkown argument: ${1}"
fi
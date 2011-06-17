gcc -I/opt/local/include -E main.cpp | ssed -R 's/{/{\n/g' | ssed -R 's/}/\n}/g' | ssed -R 's/;/;\n/' | grep -v "^$" | uncrustify -l CPP -c uncrustify-config


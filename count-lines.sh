find . -not -path './include*' -not -path './engine/lua/*'  -not -path './projects/testgame/*' -not -path './engine/oak/assets/stb_image.h' -a \( -name '*.cpp' -o -name '*.h' \) | xargs wc -l
read -p "Press enter to continue..."

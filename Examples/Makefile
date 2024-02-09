INCLUDE_DIR=-I../.

all: example area_filtering areaFiltering16b simple_example 

example: example.cpp
	g++ -O3  $(INCLUDE_DIR) example.cpp -o example
area_filtering: area_filtering.cpp
	g++ -O3  $(INCLUDE_DIR) area_filtering.cpp -o area_filtering
areaFiltering16b: areaFiltering16b.cpp
	g++ -O3  $(INCLUDE_DIR) areaFiltering16b.cpp -o areaFiltering16b
simple_example: simple_example.cpp
	g++ -O3  $(INCLUDE_DIR) simple_example.cpp -o simple_example
# attribute_image: attribute_image.cpp
# 	g++ -O3  $(INCLUDE_DIR) attribute_image.cpp -o attribute_image


clean:
	rm example area_filtering  areaFiltering16b simple_example attribute_image 


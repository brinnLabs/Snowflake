# Snowflake
A port of Evil Mad Scientist's snowflake maker into openframeworks http://www.evilmadscientist.com/2008/vector-snowflake-application/

This just takes the processing code into openframeworks and adds in a few features like:
* determining the polywinding mode 
* instead of exporting to a pdf it exports to an SVG. 
* Slider for number of sides

What is polywinding?
This website has a pretty good explanation of the concept
http://www.sitepoint.com/understanding-svg-fill-rule-property/

Basically it determines holes in a shape based on how the lines overlap. There are more than just the ones this program has but since we are exporting to an svg they only have either even-odd or non-zero windings

If you want to build this yourself download openframeworks
http://www.openframeworks.cc/

use the project generator to make a new project, you can use the src files included in this repo. I had to edit some files in order to make things work, so I am including the edited addons as well

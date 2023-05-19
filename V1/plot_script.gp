# Set the data file
datafile = "data.txt"

# Set the datafile separator
set datafile separator ","

# Set the terminal and output file
set term png
set output "result.png"

# Find the highest value
stats datafile using 2 nooutput
max_value = STATS_max

# Set the y-label range
set yrange [0:max_value+15]

# Set title and labels
set grid
set title "Scores"
set xlabel "Rounds"
set ylabel "Snake length's"

# Set line and point styles
set style line 1 lc rgb "#FF0000" lw 2 pt 7 ps 1
set style line 2 lc rgb "#0000FF" lw 2 pt 7 ps 1

# Define legends
data_legend = "Progress"
max_value_legend = "Value"

# Declare and initialize point_style variable
point_style = 1

# Plot the data
plot datafile using 2:1 with linespoints ls 1 title data_legend, \
     "" using 2:1:(max_value == $1 ? $1 : NaN):(max_value == $1 ? $1 : NaN):(max_value == $1 ? 2 : 1) with points ls point_style title max_value_legend

# Wait for keypress to close the plot
pause -1 "Hit any key to continue"

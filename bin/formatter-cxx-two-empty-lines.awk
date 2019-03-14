
#
# Helper for C++ code formatting. 
# Ensures that there are two new empty lines between function definitions that the namespace level.
#

BEGIN {
    x = 0
    level = 0
}
{
    if ($0 ~ /^{/) {
        empty_lines = 0
        x = 0
        print $0
    }
    else if ($0 ~ /^}/) {
        x = 1
        empty_lines = 0
        print $0
    }
    else if ($0 ~ /^$/) {
        empty_lines++
        print $0
    }
    else if ($0 ~ /^[a-zA-Z]/) {
        if (x == 1) {
            if (empty_lines == 0) {
                print "\n\n" $0
            } else if (empty_lines == 1) {
                print "\n" $0
            }
            else {
                print $0
            }
            x = 0
        } else {
            print $0
        }
        empty_lines = 0
    }
    else {
        empty_lines = 0
        print $0
    }
}

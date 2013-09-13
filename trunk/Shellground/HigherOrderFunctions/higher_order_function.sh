increment() { echo $(($1 + 1)) ; }
decrement() { echo $(($1 - 1)) ; }
apply() { $1 $2 ; }

apply increment 100
apply decrement 100

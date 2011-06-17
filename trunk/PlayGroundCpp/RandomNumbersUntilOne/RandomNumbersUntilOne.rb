# Count number of random numbers needed
def random_number_count_until_one
	count = 1
	sum = rand	
	while sum < 1
		sum += rand # rand function returns a random number between 0 and 1
		count += 1
	end
	return count
end

def count_average(n)
	count = 0
	n.times do     
		count += random_number_count_until_one
	end
	return count.to_f / n.to_f
end

n = 100000
puts "Correct value of e is  2.71828182845904523536..."
print "Our approximization is "
puts count_average(n)

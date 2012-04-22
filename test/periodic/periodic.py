import time

def read_periodic_table(filename):
	f = open(filename)
	lines = f.readlines()
	tbl = []
	for line in lines:
		line = line.strip()
		i = 0
		for c in line:
			if c.isspace():
				break
			i = i + 1
		abbr = line[:i]
		full_name = line[i:].strip()
		tbl.append((abbr, full_name))
	return tbl

def first_n(word, size):
	if len(word) < size:
		return ""
	return word[:size]

def truncate_word(word, size):
	return word[size:]

def strcasecmp(a, b):
	return a.lower() == b.lower()

def is_periodic_word(tbl, word):
	if len(word) == 0:
		return True
	for entry in tbl:
		abbr = entry[0]
		subword = first_n(word, len(abbr))
		if strcasecmp(abbr, subword):
			restword = truncate_word(word, len(abbr))
			if is_periodic_word(tbl, restword):
				return True
	return False

def print_timing(func):
    def wrapper(*arg):
        t1 = time.clock()
        res = func(*arg)
        t2 = time.clock()
        print '%s took %0.3f ms' % (func.func_name, (t2-t1)*1000.0)
        return res
    return wrapper

test = ("how", "when", "where", "what", "who", "bamboo", "crunchy",
		"finance", "genius", "tenacious")
tbl = read_periodic_table("periodic-table")

@print_timing
def dict_periodic_percentage(tbl):
    fin = open("words")
    lines = fin.readlines()
    total, periodic = 0, 0
    for line in lines:
        word = line.strip()
        if len(word) == 0:
            continue
        if is_periodic_word(tbl, word):
            periodic = periodic + 1
        total = total + 1
    print periodic, total, (periodic + 0.0) / total

dict_periodic_percentage(tbl)


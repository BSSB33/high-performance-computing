import json
import re

f = open('yelp_academic_dataset_review.json', encoding='utf-8')
out = open("output2.txt", "w", encoding="utf-8")

counter = 0
for line in f:
    line = line.split("\"text\":\"",1)[1].split("\",\"date\":\"", 1)[0]
    line = re.sub("\\n","", line)
    line = line.replace("\\n", " ").replace("\\n\\n", " ").replace(" \\n\\n", " ").replace(" \\ ", " ").replace("\\", " ").replace("\/", " ")
    counter += 1
    out.write(str(line) + "\n")

    gb1 = 1000000
    mb5 = 10000
    if(counter == mb5): break
    

f.close()
out.close()

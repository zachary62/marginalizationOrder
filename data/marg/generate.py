f = open("R.csv" , "w")
f.write("A,B,C,D,E\n")
a = 1
b = 1
c = 1
d = 1
e = 1
for i in range(1,100001,1):
    f.write(str(a) + "," + str(b) +  "," + str(c) +  "," + str(d) + "," + str(e) + "\n")

    e = e + 1
    if(e > 10):
        e = 1
        d = d + 1

    if(d > 10):
        d = 1
        c = c + 1

    if(c > 10):
        c = 1
        b = b + 1

    if(b > 10):
        b = 1
        a = a + 1    
    


f.close()

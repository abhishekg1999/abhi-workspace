## APIS 

int p[2];
pipe(p);

read(p[0], buff, sizeof(buff));   
write(p[1], buff, strlen(buff)+1);

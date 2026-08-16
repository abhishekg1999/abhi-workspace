int main()
{
	int a[5]={1,2,3,4,5};
	int i,temp,flag,roll;
	
	printf("how many times want to rotate\n");
	scanf("%d",&roll);
	
	while(roll--)
	{
	flag=1;
	for(i=0;i<5;i++){
		if(flag==1){
		temp=a[0];
		flag=0;
		}
		a[i]=a[i+1];
		}
	a[4]=temp;
	}
	
}

int main()
{
	int a[ROW][COL]={{1,2,3},{4,5,6},{7,8,9}};
	int i,j,m,n,b[ROW][COL];
	
	for(j=COL-1,m=0;j>=0;j--,m++)
	for(i=0,n=0;i<ROW;i++,n++)
		b[m][n]=a[i][j];
}

int main()
{
	int ele,i,j,a[5]={1,2,3,4,5};
	ele=sizeof(a)/sizeof(a[0]);

	for(i=0.j=ele-1;i<j;i++,j--)
		a[i]=a[i]+a[j]-(a[j]=a[i]);
}

int main()
{
	int i,j,k,ele;
	int a[8]={1,2,3};
	ele=sizeof(a)/sizeof(a[0]);
	
	for(i=0;i<ele;i++)
	{
	for(j=i+1;j<ele;j++)
		if(a[i]==a[j]){
		for(k=j;k<ele;k++)
		a[k]=a[k+1];
		j--;
		ele--;
		}	
	}
}

int main()
{
	int i,l,sl,a[5]={10,20,30,40,50};
	
	if(a[0]>a[1]){
	l=a[0];
	sl=a[1];
	}
	else{
	l=a[1];
	sl=a[0];
	}

	for(i=2;i<5;i++){
		if(a[i]>l){
		sl=l;
		l=a[i];
		}
		else if(a[i]>sl){
		if(a[i]!=l)
			sl=a[i];
		}
	}
}

int main()
{
	int i,count=0,a[10]={1,2,3,4--};
	int num,s,ele;
	ele=sizeof(a)/sizeof(a[0]);
	
	for(i=0;i<ele;i++){
		for(num=a[i],s=0;num;num=num/10)
		s=s*10+num%10;
		
		if(s==a[i]){
		count++;
	}
}

int main()
{
	int i,j,a[10]={11,12,22,13,14,15,16,44,55,66};
	int ele,count=0;
	ele=sizeof(a)/sizeof(a[0]);

	for(i=0;i<ele;i++){
			for(j=2;j<a[i];j++){
			if(a[i]%j==0)
			break;
			}
		if(j==a[i])
		count++;
	}
}

int main()
{
for(pos=31;pos>=0;pos--)
	if(num>>pos&1)
	count++;
}

int main()
{ 
	num=num|1<<pos; //set 
	num=num&~(1<<pos); //clear
	num=num^1<<pos; // complement
}

int main()
{
	for(i=0,j=31;i<j;i++,j--){
	m=num>>i&1;
	n=num>>j&1;
		
		if(m!=n){
		num=num^1<<i;
		num=num^1<<j;
		}
	}
}	


typedef int(*FPTR)(int,int);

int main()
{
	FPTR p;
	int k;
	p=func();
	k=(*p)(10,20);
}

FPTR func(void)
{
	return sum;
}

int call_back(int m,int n,FPTR p)
{
	int r;
	r=(*p)(m,n);
	return r;
}



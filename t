# Fichier de test
# Les lignes de commentaires commencent par #
procedure principale()
  i = 12+3;
  j=0;
  si (i>14)
    j=1;
  sinonsi (j==3)
    j=3;
  sinonsi(j>4)
    lire(i)
    sinon
    ecrire(i,99999,"test")
  finsi
    ecrire(i,111111)
tantque(j<42)
    j=j+1;
 fintantque
repeter
j=j-10;
jusqua ( j<-1000 )
i=0;
k=0;
#penser a reparer le pour, finir le ecrire et lire
pour(i=0;i<10;)
k=k-1000;
i=i+1;
ecrire("test=",i," ",555," ",i," ",666)
ecrire(1)
finpour
lire(k,l,m)
ecrire(k)
ecrire(k,l)
ecrire(k,l,m)
ecrire(k,l)
ecrire(k)
finproc
# Commentaire Final
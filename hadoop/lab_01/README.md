# Passo a passo

1. Atividando Hadoop

```
ssh -p 13508 <usuario>@chococino.naquadah.com.br
```

```
source /home/prof/hadoop/bin/chococino_env

ssh cm2
```

2. Verificar hadoop versioes

```
hadoop version && java --version && javac -version
```

3. Criando arquivo de input

```
mkdir input_data && cd input_data && nano input.txt
```
Insirei os nome desejados

4. Criação no hadoop

```
hdfs dfs -mkdir input_data
```

5. Inserindo input no hadoop

```
hdfs dfs -put <nome_arquivo_local> <nome_arquivo_no_hdfs>
```

6. Criando o arquivo de contar palavras

```
nano WordCount.java
```
copia o [codigo](https://www.dropbox.com/s/yp9i7nwmgzr3nkx/WordCount.java?dl=0) para o WordCount.java

7. Compilando o arquivo

```
mkdir classes && javac -target 8 -source 8 -cp `hadoop classpath` -d classes WordCount.java -nowarn
```
obs: target e source define o java utilizado na compilacao e o -cp é a classepath utilizada que é o comando hadoop classpath que retorna o print do diretiro do path

8. Tranformando em jar

```
jar -cvf contadorPalavras.jar -C classes/ .
```

9. Executando o hadoop

```
hadoop jar contadorPalavras.jar WordCount /user/<usuario>/input_data /user/<usuario>/output_data
```
obs: a pasta de saida nao pode ser repitida e nem crianda antes ele cria na hora de salvar o resultado.

10. Visualizacao

```
 hdfs dfs -ls output_data
 hdfs dfs -cat output_data/part-r-00000
```
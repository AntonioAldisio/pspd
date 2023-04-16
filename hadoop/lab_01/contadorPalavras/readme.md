# Contador de Palavras

## Passo a Passo

1. Compilar o WordCount.java

```bash
make compila
```

2.Transformar .jar

```bash
make jar
```

3.Rodar o Hadoop

```bash
hadoop jar WordCount.jar WordCount <diretorio da entrada> <diretorio do output>
```

4. Visualizar saída

```bash
 hdfs dfs -cat <diretorio do output>/part-r-00000
```
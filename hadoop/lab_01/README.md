# hadoop chococino

1. Atividando Hadoop

```bash
ssh -p 13508 <usuario>@chococino.naquadah.com.br
```

```bash
ssh cm2 && source /home/prof/hadoop/bin/chococino_env
```

2. Verificar hadoop versioes

```bash
hadoop version && java --version && javac -version
```

3. Criando arquivo de input

```bash
mkdir input_data && cd input_data && nano input.txt
```
Insirei os nome desejados

4. Criação no hadoop

```bash
hdfs dfs -mkdir input_data
```

5. Inserindo input no hadoop

```bash
hdfs dfs -put <nome_arquivo_local> <nome_arquivo_no_hdfs>
```

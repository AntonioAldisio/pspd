putFileInCluster:
	hdfs dfs -put redeAmizades.txt /user/cloudera/lab_01/redeAmizades

compila:
	javac -target 8 -source 8 -cp `hadoop classpath` -d classes FriendsCount.java -nowarn

jar:
	jar -cvf FriendsCount.jar -C classes/ .
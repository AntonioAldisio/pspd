import java.io.IOException;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class MyMapReduce {
    public static class MyMapper extends Mapper<Object, Text, Text, Text> {
        private Text entrada = new Text();
        private Text saida = new Text();

        public void map(Object key, Text value, Context context)
                throws IOException, InterruptedException {
            String[] valores = value.toString().split(",");
            if (valores.length == 2) {
                entrada.set(valores[0]);
                saida.set(valores[1]);
                context.write(entrada, saida);
            }
        }
    }

    public static class MyReducer extends Reducer<Text, Text, Text, Text> {
        public void reduce(Text key, Iterable<Text> values, Context context)
                throws IOException, InterruptedException {
            StringBuilder sb = new StringBuilder();
            for (Text value : values) {
                sb.append(value.toString()).append(" ");
            }
            String saidas = sb.toString().trim();
            context.write(key, new Text(saidas));
        }
    }

    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        Job job = Job.getInstance(conf, "MyMapReduce");
        job.setJarByClass(MyMapReduce.class);
        job.setMapperClass(MyMapper.class);
        job.setReducerClass(MyReducer.class);
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(Text.class);
        FileInputFormat.addInputPath(job, new Path(args[0]));
        FileOutputFormat.setOutputPath(job, new Path(args[1]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}

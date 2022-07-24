import java.net.ServerSocket;
import java.net.Socket;

public class Server implements Runnable{

    private int port=8088;
    public Server(int port){
        this.port=port;
    }
    @Override
    public void run() {
        ServerSocket serverSocket=null;
        try {
            serverSocket=new ServerSocket(port);
            System.out.println("开始监听......");
            while (Data.isRun) {
                Socket socket = serverSocket.accept();
                System.out.println("接收到请求...");

                RequestExecute re=new RequestExecute(socket);

                re.start();
            }

            serverSocket.close();
            serverSocket=null;

        }catch (Exception e){
            e.printStackTrace();
            throw  new RuntimeException("端口"+port+"监听失败"+e.getMessage());
        }
    }
    public static void main(String[] args) {
        Server server=new Server(8088);
        new Thread(server).start();
    }

}



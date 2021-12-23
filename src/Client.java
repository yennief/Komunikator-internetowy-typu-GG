import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {

    private Socket clientSocket;
    private BufferedReader reader;
    private BufferedWriter writer;
    private String username;


    public Client(Socket socket, String username) {
        try{
            this.clientSocket = socket;
            this.writer = new BufferedWriter(new OutputStreamWriter(clientSocket.getOutputStream()));
            this.reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            this.username = username;
        }catch(IOException e){
            close(clientSocket,reader,writer);
        }

    }
    public void send_message(){
        try{
            writer.write(username);
            writer.newLine();
            writer.flush();

            Scanner scanner = new Scanner(System.in);
            while(clientSocket.isConnected()){
                String message = scanner.nextLine();
                if(message.equals("quit")){
                    writer.write(message);
                    writer.newLine();
                    writer.flush();
                    close(clientSocket,reader,writer);
                    break;
                }
                else{
                    writer.write(username + ": " + message);
                    writer.newLine();
                    writer.flush();
                }


            }
        }catch(IOException e){
            close(clientSocket,reader,writer);

        }
    }
    public void receive_message(){
        new Thread(new Runnable(){
            @Override
            public void run(){
                String message;

                while(clientSocket.isConnected()){
                    try{
                        message = reader.readLine();
                        if(message.equals("quit")) {
                            close(clientSocket,reader,writer);
                            break;
                        }
                        System.out.println(message);
                    }catch(IOException e){
                        close(clientSocket,reader,writer);
                    }
                }
            }
        }).start();
    }

    public void close(Socket clientSocket, BufferedReader reader,BufferedWriter writer){
        try{
            if(clientSocket !=null){
                clientSocket.close();
            }
            if(reader !=null){
                reader.close();
            }
            if(writer !=null){
                writer.close();
            }
        }catch(IOException e){
            e.printStackTrace();
        }
    }

}

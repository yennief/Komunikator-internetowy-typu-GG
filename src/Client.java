import javax.swing.*;
import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;

public class Client {

    private static Socket clientSocket = null;
    private static String host="localhost";
    private static int port=1234;
    private BufferedReader reader;
    private PrintWriter writer;
    private String username;
    private String userPassword;


    public Client(Socket socket, String username, String userPassword) {
        try{
            if(username.equals("") || userPassword.equals("")) {
                throw new Exception("Didn't creat user");
            }
            this.clientSocket = socket;
            this.writer = new PrintWriter(clientSocket.getOutputStream(), true);
            this.reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            this.username = username;
            this.userPassword = userPassword;


        }catch(Exception e){
            close(clientSocket,reader,writer);
        }

    }

    public void getnewClientSocket() throws IOException {
        this.clientSocket.close();
        Socket clientSocket_new = new Socket(Client.host,Client.port);
        this.clientSocket=clientSocket_new;
        this.writer = new PrintWriter(clientSocket.getOutputStream(), true);
        this.reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

    }

    public String checkLogin(){

        String serverMessage="";

        try {

            writer.println("L"+"\t"+username+"\t"+userPassword+"\t");

            serverMessage = reader.readLine();

            System.out.printf(serverMessage+"\n");
            if (!serverMessage.equals("User not found") && !serverMessage.equals("Wrong password")) {
                //user is logged in -> add their friends list
            }

        } catch (IOException e) {
            e.printStackTrace();
        }

        return serverMessage;


    }

    public String register() throws IOException {

        String serverMessage="";

        writer.println("R"+"\t"+username+"\t"+userPassword+"\t");

        serverMessage = reader.readLine();

        System.out.printf(serverMessage+"\n");

        return serverMessage;
    }

    public void close(Socket clientSocket, BufferedReader reader,PrintWriter writer){
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

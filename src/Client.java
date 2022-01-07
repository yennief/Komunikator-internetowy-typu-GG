import javax.swing.*;
import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

public class Client {

    private static Socket clientSocket = null;
    private static String host="localhost";
    private static int port=1234;
    private BufferedReader reader;
    private PrintWriter writer;
    private String username;
    private String userPassword;
    private List<String> friends_list;
    private boolean mutex=true;


    public Client(Socket socket, String username, String userPassword) {
        try{
            if(username.equals("") || userPassword.equals("")) {
                throw new Exception("Didn't creat user");
            }
            this.clientSocket = socket;
            //this.writer = new PrintWriter(clientSocket.getOutputStream(), true);
            //this.reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            this.username = username;
            this.userPassword = userPassword;
            this.friends_list= new ArrayList<String>();


        }catch(Exception e){
            close(clientSocket);
        }

    }

    public void new_socket() throws IOException {
        this.clientSocket.close();
        Socket clientSocket_new = new Socket(Client.host,Client.port);
        this.clientSocket=clientSocket_new;
        //this.writer = new PrintWriter(clientSocket.getOutputStream(), true);
        //this.reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

    }

    public String checkLogin() throws IOException{

        String serverMessage="";
        String list;
        String empty_or_full;


            PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);

            writer.println("L"+"\t"+username+"\t"+userPassword+"\t");

            BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

            serverMessage = reader.readLine();

            //System.out.printf("CHECK \n");
            System.out.printf(serverMessage+"\n");
            if (!serverMessage.equals("User not found") && !serverMessage.equals("Wrong password")) {

                empty_or_full = reader.readLine();
                //System.out.printf(empty_or_full.trim()+"\n");
                if(empty_or_full.trim().equals("full")){

                    //System.out.printf("ODCZYTALEM FULL \n");
                    list = reader.readLine();
                    //System.out.printf(list+"\n");

                    this.friends_list =  new ArrayList<>(Arrays.asList(list.split("\t")));
                }
                else if(empty_or_full.trim().equals("empty")){

                    this.friends_list =  new ArrayList<>();
                }

            }

        return serverMessage;

    }

    public String register() throws IOException {

        String serverMessage="";

        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);

        writer.println("R"+"\t"+username+"\t"+userPassword+"\t");

        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

        serverMessage = reader.readLine();

        System.out.printf(serverMessage+"\n");

        return serverMessage;
    }

    public String receive_msg() throws IOException {

        String serverMessage="";
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        if(mutex && reader.ready()) {
        serverMessage = reader.readLine();
        }
        return serverMessage;
    }


    public boolean add_friend(String username_to_add) throws IOException {

        mutex=false;
        String serverMessage="";

        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);

        writer.println("A"+"\t"+username_to_add+"\t");

        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

        serverMessage = reader.readLine();

        if(serverMessage.equals("Added friend")){
            friends_list.add(username_to_add);
        }

        System.out.println(serverMessage);
        mutex=true;
        return true;

    }

    public void close(Socket clientSocket){
        try{
            if(clientSocket !=null){
                clientSocket.close();
            }

        }catch(IOException e){
            e.printStackTrace();
        }
    }

    public String getUsername() {
        return username;
    }

    public List<String> getFriends_list() {
        return friends_list;
    }

    public void setFriends_list(List<String> friends_list) {
        this.friends_list = friends_list;
    }
}

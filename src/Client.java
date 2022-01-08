import javax.swing.*;
import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Scanner;

public class Client {

    private static Socket clientSocket = null;
    private String username;
    private String userPassword;
    private List<String> friends_list;
    private boolean mutex=true;


    public Client(Socket socket, String username, String userPassword) {
        try{
            if(username.equals("") || userPassword.equals("")) {
                throw new Exception("User not created");
            }
            this.clientSocket = socket;
            this.username = username;
            this.userPassword = userPassword;
            this.friends_list= new ArrayList<String>();

        }catch(Exception e){
            close(clientSocket);
        }

    }

    public void new_socket(String host,int port) throws IOException {
        this.clientSocket.close();
        Socket clientSocket_new = new Socket(host,port);
        this.clientSocket=clientSocket_new;

    }

    public String checkLogin() throws IOException{

        String get_message="";
        String list;
        String empty_or_full;

        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("L"+"\t"+username+"\t"+userPassword+"\t");
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        get_message = reader.readLine();
        System.out.printf(get_message+"\n");
        if (!get_message.equals("User not found") && !get_message.equals("Wrong password")) {
            empty_or_full = reader.readLine();
            if(empty_or_full.trim().equals("full")){
                list = reader.readLine();
                this.friends_list =  new ArrayList<>(Arrays.asList(list.split("\t")));
            }
            else if(empty_or_full.trim().equals("empty")){
                this.friends_list =  new ArrayList<>();
            }

        }

        return get_message;

    }

    public String register() throws IOException {

        String get_message="";
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("R"+"\t"+username+"\t"+userPassword+"\t");
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        get_message = reader.readLine();
        //System.out.printf(serverMessage+"\n");
        return get_message;
    }

    public String add_friend(String username_to_add) throws IOException {

        mutex=false;
        String get_message="";
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("A"+"\t"+username_to_add+"\t");
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        get_message = reader.readLine();

        if(get_message.equals("Added friend")){
            friends_list.add(username_to_add);
        }

        mutex=true;
        return get_message;

    }

    public String delete_friend(String username_to_delete) throws IOException {

        mutex=false;
        String get_message="";
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("D"+"\t"+username_to_delete+"\t");
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        get_message = reader.readLine();

        if(get_message.equals("Deleted friend")){
            friends_list.remove(username_to_delete);
        }

        mutex=true;
        return get_message;

    }

    public boolean send_message(String to_who, String message) throws IOException {

        mutex=false;
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("M"+"\t"+to_who+"\t"+message+"\t");
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        String get_message = reader.readLine();
        if(get_message.equals("sent")){
            System.out.println(get_message);

        }
        mutex=true;
        return true;

    }

    public String load_conversation(String from_who) throws IOException {

        mutex=false;
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("l"+"\t"+from_who+"\t");
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        String get_message = "";
        String message="";
        get_message = reader.readLine();

        while(!get_message.equals("stop")) {
            if(!get_message.equals("")) {
                message += get_message.substring(2);
            }
            message+="\n";
            get_message = reader.readLine();

        }
        mutex=true;
        return message;

    }

    public String receive_msg() throws IOException {

        String get_message="";
        BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        if(mutex && reader.ready()) {
            get_message = reader.readLine();
        }
        return get_message;
    }

    public void log_out() throws IOException {
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println("Q"+"\t");

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

    public String getFriend(int idx){
        return friends_list.get(idx);
    }

    public void setFriends_list(List<String> friends_list) {
        this.friends_list = friends_list;
    }
}

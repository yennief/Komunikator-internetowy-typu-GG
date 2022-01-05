import java.awt.*;
import java.awt.event.ActionEvent;
import java.net.*;
import java.io.*;
import java.util.Scanner;
import javax.swing.*;
import javax.swing.border.EmptyBorder;

public class Application{


    public Application() throws Exception {

        JFrame frame = new JFrame("GG");
        StartWindow start = new StartWindow();

        frame.setContentPane(start.getPanel());
        start.setFrame(frame);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setSize(500,400);
        frame.setVisible(true);

    }

    public class MyRunnable implements Runnable{
        @Override
        public void run(){


            try {


                while (true) {


                }
            } catch (Exception e) {

            }

        }
    }
}

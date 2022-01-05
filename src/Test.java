
import java.awt.EventQueue;
import java.io.IOException;

public class Test {
    public static void main(String[] args){
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {

                    try {

                        new Application();

                    } catch (Exception e) {
                        e.printStackTrace();
                    }

            }
        });
    }

}

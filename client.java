import java.io.*;
import gnu.getopt.Getopt;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.channels.SocketChannel;
import java.util.logging.Level;
import java.util.logging.Logger;


class client {
	
	// Write attributes here
	
	private static boolean _debug = false;
	private static String _server = null;
	private static int _port = -1;
        private static String localIpAddrStr = "";

	// Write methods here
	
	static void f_ping(){

            String fromServer;
            
            try {
            Socket clientSocket = new Socket(_server,_port);

            // Send and receive buffers
            OutputStream out = clientSocket.getOutputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));



            double init = System.currentTimeMillis(); // Start timer

            //Send operation Code
            out.write("0".getBytes());
            //out.flush();
            //out.write(0);
            String ipsent = client.localIpAddrStr;
            ipsent += " ping";
            //Send ip
            out.write(ipsent.getBytes());
            
            //Receive ack, end timer and print result
            while ((fromServer = in.readLine()) != null) {
                if (fromServer != null)
                {
                    double end = System.currentTimeMillis();
                    System.out.println((end - init)/(2 * 1000) + " s");
                    break;
                }
            }
            System.out.println("Done");
            out.close();
            in.close();
            clientSocket.close();

            }
            catch (IOException e) {
                System.out.println("c> Error connecting to server " + _server + ":" + _port);
                System.exit(-1);
            }
	}

        
	static void f_swap(String src, String dst){
        try {
           
            Socket socket = new Socket (_server, _port);


		//Se crean los buffers para poder enviar y recibir datos
            DataOutputStream out = new DataOutputStream (socket.getOutputStream());
            DataInputStream in = new DataInputStream (socket.getInputStream());

            
            // Send and receive buffers
            //OutputStream out = clientSocket.getOutputStream();
            //BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));


            //////////// Object declarations/////////////////////////////////////
            ByteBuffer bb = ByteBuffer.allocate(500);
            char cbuf[] = new char[500];
            File swapFile = new File(src);
            FileReader fr = new FileReader(swapFile);
            /////////////////////////////////////////////

            short opno = '1';

            bb.clear();

            bb.order(ByteOrder.LITTLE_ENDIAN);

            bb.putShort(opno);

            bb.flip();

            //System.out.println("Buffer status " + bb.toString());
            String sentip = client.localIpAddrStr;


            out.write(opno);
            out.flush();
            //in.readByte();
            
            //in.reset();
            out.writeBytes(sentip);
            out.flush();
            //in.readByte();
            //in.reset();


            System.out.println("sent " + sentip);

            //in.readByte();

            
            /////////// Send file length ///////////////////////
            int fileSize =  (int)swapFile.length();
            if(fileSize == 0)throw new IOException();

            out.writeInt(fileSize);
            out.flush();
            System.out.println("sent long " + fileSize);
//            bb.clear();
//            bb.order(ByteOrder.BIG_ENDIAN);
//            bb.putInt(fileSize);
//            bb.put(0, (byte)0);
//            bb.flip();
//            System.out.println(bb.toString());
//
//            numwritten = 0;
//            while(numwritten == 0)
//            {
//                //numwritten = schannel.write(bb);
//            }
            ////////////////////////////////////////////

            long init = 0;
            long end = 500;

            while(fileSize > 0)
            {
                if(fileSize < end) end = fileSize;
                ///// Read chunk from file //////////////////
                fr.read(cbuf, 0,500);
                System.out.println(fileSize + " " + (new String(cbuf)));
                byte[] chunk = (new String(cbuf)).getBytes();
                ////////////////////////////////////////////////////

                //////// Send chunk to server ///////////////////
                bb.clear();
                bb.put(chunk,0,chunk.length);
                bb.flip();
                //numwritten = 0;
                //while(numwritten == 0)
                //{
                    //schannel.write(bb);
                //}
                System.out.println("Sent");
                /////////////////////////////////////////////////

                //////// Receive number of swapped letters ////////

                ///////////////////////////////////////////////////

                ///////// Receive swapped chain ///////////////////

                ////////////////////////////////////////////////////



                init+=500;
                end+=500;
                fileSize-=500;

            }

        } catch (UnknownHostException ex) {
            Logger.getLogger(client.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(client.class.getName()).log(Level.SEVERE, null, ex);
        }
	
		
	}

	static void f_hash(String src){
		if (_debug)
			System.out.println("HASH <SRC=" + src + ">");
	
		// Write code here
	}

	static void f_check(String src, int hash){
		if (_debug)
			System.out.println("CHECK <SRC=" + src + "> <HASH=" + hash + ">");
	
		// Write code here
	}

        /*TODO: Implement */
	static void f_stat() throws InterruptedException{

            try {

            int readnumber = -1;
            SocketAddress address = new InetSocketAddress(_server, _port);
            
            SocketChannel schannel = SocketChannel.open(address);
            schannel.configureBlocking(false);
            //System.out.println("Connected=" + schannel.isConnected());
            //schannel.socket().bind(address);


            // Allocate a big endian byte buffer
            ByteBuffer bb = ByteBuffer.allocate(25);
            //bb.order(ByteOrder.nativeOrder());

            short opno = '4';

            bb.clear();
            
            bb.order(ByteOrder.LITTLE_ENDIAN);

            bb.putShort(opno);

            bb.flip();
 
            //System.out.println("Buffer status " + bb.toString());
            String sentip = client.localIpAddrStr;
            
            /////////////SEND OPERATION //////////////////////////////////
            int numwritten = 0;
            while(numwritten  == 0)
            {
                numwritten = schannel.write(bb);
            }
            ////////////////////////////////////////////////////

            bb.clear();
            bb.put(sentip.getBytes(),0,sentip.getBytes().length);
            bb.flip();

            ///////// SEND IP //////////////////////// 
            numwritten = 0;
            while(numwritten  == 0)
            {
                numwritten = schannel.write(bb);
            }      
            //////////////////////////////////////////////

            //////////////// Receive ping //////////////////////////
            bb.clear();
            bb.order(ByteOrder.LITTLE_ENDIAN);

            while ((readnumber = schannel.read(bb)) <= 0) {

            }
            if(readnumber != 10) throw new IOException("Something went very bad while reading");
            bb.order(ByteOrder.BIG_ENDIAN);
            bb.flip();
            //System.out.println("Received ping " + readnumber);
            System.out.println("ping " + bb.getShort(0));
            System.out.println("swap " + bb.getShort(2));//Swap
            System.out.println("hash " + bb.getShort(4));//Hash
            System.out.println("check " + bb.getShort(6));//Check
            System.out.println("stat " + bb.getShort(8));//Stat
            ///////////////////////////////////////////////

            bb = null;
            schannel.socket().shutdownInput();
            schannel.socket().shutdownOutput();

            schannel.close();
            schannel.socket().close();
            System.out.println("closed=" + schannel.socket().isClosed());

            }
            catch (IOException e) {
                System.out.println("c> Error connecting to server " + _server + ":" + _port);
                System.exit(-1);
            }
	
		// Write code here
	}

	static void f_quit(){
		if (_debug)
			System.out.println("QUIT");

                System.exit(0);
		// Write code here
	}

	static void usage() {
		System.out.println("Usage: java -cp . client [-d] -s <server> -p <port>");
	}
	
	static boolean parseArguments(String [] argv) {
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				case 'd':
					_debug = true;
					break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}
	
	static void shell() throws InterruptedException {
		boolean exit = false;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				line = in.readLine().split("\\s");

				if (line.length > 0) {
					if (line[0].equals("ping")) {
						if  (line.length == 1)
							f_ping();
						else
							System.out.println("Syntax error. Use: ping");
					} else if (line[0].equals("swap")) {
						if  (line.length == 3)
							f_swap(line[1], line[2]);
						else
							System.out.println("Syntax error. Use: swap <source_file> <destination_file>");
					} else if (line[0].equals("hash")) {
						if  (line.length == 2)
							f_hash(line[1]);
						else
							System.out.println("Syntax error. Use: hash <source_file>");
					} else if (line[0].equals("check")) {
						if  (line.length == 3)
							try {
								f_check(line[1], Integer.parseInt(line[2]));
							} catch (java.lang.NumberFormatException e) {
								System.out.println("Syntax error: second argument must be numeric");
								System.out.println("         Use: check <source_file> <hash>");
							}
						else
							System.out.println("Syntax error. Use: check <source_file> <hash>");
					} else if (line[0].equals("stat")) {
						if  (line.length == 1)
							f_stat();
						else
							System.out.println("Syntax error. Use: stat");
					} else if (line[0].equals("quit")) {
						if  (line.length == 1) {
							f_quit();
							exit = true;
						} else {
							System.out.println("Syntax error. Use: quit");
						}
					} else {
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	public static void main(String[] argv) {
		if(!parseArguments(argv)) {
			usage();
			return;
		}
		
		if (_debug)
			System.out.println("SERVER: " + _server + " PORT: " + _port);
        try {
            InetAddress localaddr = InetAddress.getLocalHost();
            byte[] localIpAddr = localaddr.getAddress();

            ///// Convert IP to dot representation

            for (int i=0; i<localIpAddr.length; i++) {
                if (i > 0) {
                    client.localIpAddrStr += ".";
                }
                client.localIpAddrStr += localIpAddr[i]&0xFF;
            }
            client.localIpAddrStr += ":" + _port;
            //////////////////////////////////////////
            shell();
        } catch (UnknownHostException ex) {
            Logger.getLogger(client.class.getName()).log(Level.SEVERE, null, ex);
        } catch (InterruptedException ex) {
            Logger.getLogger(client.class.getName()).log(Level.SEVERE, null, ex);
        }
	}
}

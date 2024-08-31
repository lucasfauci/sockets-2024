using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

class Cliente
{
    const int DEFAULT_BUFLEN = 512;
    const string DEFAULT_PORT = "5001";
    const string DEFAULT_SERVER = "127.0.0.1";

    static void Main(string[] args)
    {
        try
        {
            IPAddress ipAddress = IPAddress.Parse(DEFAULT_SERVER);
            IPEndPoint remoteEP = new IPEndPoint(ipAddress, int.Parse(DEFAULT_PORT));

            Socket sender = new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                Console.WriteLine("Intentando establecer conexión con el servidor...");
                sender.Connect(remoteEP);

                Console.WriteLine("Conexión establecida.");

                string sendbuf = string.Empty;
                string recvbuf;

                do
                {
                    sendbuf = string.Empty;
                    Menu(ref sendbuf);

                    byte[] msg = Encoding.ASCII.GetBytes(sendbuf);
                    int bytesSent = sender.Send(msg);

                    byte[] bytes = new byte[DEFAULT_BUFLEN];
                    int bytesRec = sender.Receive(bytes);

                    recvbuf = Encoding.ASCII.GetString(bytes, 0, bytesRec);
                    if (recvbuf != "END")
                    {
                        Console.WriteLine("Mensaje del servidor: {0}", recvbuf);
                    }
                    else
                    {
                        Console.WriteLine("Conexión cerrada.");
                        break;
                    }

                } while (recvbuf != "END");

                sender.Shutdown(SocketShutdown.Both);
                sender.Close();
            }
            catch (ArgumentNullException ane)
            {
                Console.WriteLine("ArgumentNullException : {0}", ane.ToString());
            }
            catch (SocketException se)
            {
                Console.WriteLine("SocketException : {0}", se.ToString());
            }
            catch (Exception e)
            {
                Console.WriteLine("Unexpected exception : {0}", e.ToString());
            }

        }
        catch (Exception e)
        {
            Console.WriteLine(e.ToString());
        }
    }

    static void Menu(ref string sendbuf)
    {
        int menuOpcion = 0;
        char opcion = '\0';
        int longitud = 0;

        do
        {
            Console.WriteLine("\nElegí una opción del menú:");
            Console.WriteLine("1 - Generar nombre de usuario");
            Console.WriteLine("2 - Generar contraseña");
            Console.WriteLine("3 - Salir");
            Console.Write("> ");

            if (int.TryParse(Console.ReadLine(), out menuOpcion))
            {
                switch (menuOpcion)
                {
                    case 1:
                        opcion = 'u';
                        break;
                    case 2:
                        opcion = 'c';
                        break;
                    case 3:
                        opcion = 's';
                        break;
                    default:
                        Console.WriteLine("Opción incorrecta.");
                        continue;
                }

                if (menuOpcion == 1 || menuOpcion == 2)
                {
                    Console.WriteLine("\nIngresá la longitud deseada:");
                    Console.Write("> ");
                    if (int.TryParse(Console.ReadLine(), out longitud))
                    {
                        sendbuf = $"{opcion}{longitud}";
                    }
                }
                else if (menuOpcion == 3)
                {
                    Console.WriteLine("Saliendo...");
                    sendbuf = $"{opcion}{longitud}";
                }
            }
        } while (menuOpcion < 1 || menuOpcion > 3);
    }
}

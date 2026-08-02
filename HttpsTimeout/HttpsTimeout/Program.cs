using System.Net;
using System.Net.Sockets;
using System.Text;

namespace HttpsTimeout
{
    internal static class Program
    {
        private static async Task Main(string[] args)
        {
            try
            {
                IPEndPoint ipEndPoint = new IPEndPoint(IPAddress.Any, 443);
                TcpListener listener = new(ipEndPoint);
                listener.Start();
                int count = 0;
                while (true)
                {
                    await listener.AcceptTcpClientAsync();
                    count++;
                    Console.Write("\r" + count);
                    Thread.Sleep(1111);
                }
            }
            catch(Exception ex)
            {
                Console.WriteLine(ex);
            }
        }
    }
}

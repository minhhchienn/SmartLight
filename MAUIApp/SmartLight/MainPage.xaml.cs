using Newtonsoft.Json;
using System.Net.Http;

namespace SmartLight
{
    public partial class MainPage : ContentPage
    {
        const string url = "http://192.168.1.155";
        
        bool led1State = false;
        bool led2State = false;

        bool connected = false;

        HttpClient client = new HttpClient();

        private Thread? backgroundThread;

        public MainPage()
        {
            client.Timeout = TimeSpan.FromSeconds(1);
            InitializeComponent();
            StartBackgroundThread();
        }

        private void StartBackgroundThread()
        {
            backgroundThread = new Thread(() =>
            {
                while (true)
                {
                    connected = PingUrlAsync(url + "/led").Result;
                    MainThread.BeginInvokeOnMainThread(() =>
                    {
                        UpdateState();
                    });
                    if (connected)
                    {
                        try
                        {
                            string response = GetAsync(url + "/led").Result;
                            var ledStates = JsonConvert.DeserializeObject<Dictionary<string, bool>>(response);
                            if (ledStates != null)
                            {
                                led1State = ledStates.ContainsKey("led1") && ledStates["led1"];
                                led2State = ledStates.ContainsKey("led2") && ledStates["led2"];
                                MainThread.BeginInvokeOnMainThread(() =>
                                {
                                    UpdateLightText();
                                });
                            }
                        }
                        catch (Exception e)
                        {

                        }
                    }
                    Thread.Sleep(100);
                }
            });
            backgroundThread.IsBackground = true;
            backgroundThread.Start();
        }

        void UpdateState()
        {
            if (connected) StatusLabel.Text = "Status: Connected";
            else 
            { 
                StatusLabel.Text = "Status: Disconnected";
                Light1StatusLabel.Text = "LED 1: Unknown";
                Light2StatusLabel.Text = "LED 2: Unknown";
            }
        }

        void UpdateLightText()
        {
            if (led1State) Light1StatusLabel.Text = "LED 1: ON";
            else Light1StatusLabel.Text = "LED 1: OFF";

            if (led2State) Light2StatusLabel.Text = "LED 2: ON";
            else Light2StatusLabel.Text = "LED 2: OFF";
        }

        public async Task<string> ToggleLed1Async()
        {
            string postUrl = url + "/toggle-led1";
            HttpContent content = new StringContent("");
            return await PostAsync(postUrl, content);
        }

        public async Task<string> ToggleLed2Async()
        {
            string postUrl = url + "/toggle-led2";
            HttpContent content = new StringContent("");
            return await PostAsync(postUrl, content);
        }

        void OnToggleLight1Clicked(object sender, EventArgs e)
        {
            ToggleLed1Async();
        }
        void OnToggleLight2Clicked(object sender, EventArgs e)
        {
            ToggleLed2Async();
        }
        private async Task<bool> PingUrlAsync(string url)
        {
            try
            {
                using (var httpClient = new HttpClient())
                {
                    httpClient.Timeout = TimeSpan.FromSeconds(1);
                    var response = await httpClient.GetAsync(url);
                    return response.IsSuccessStatusCode;
                }
            }
            catch
            {
                return false;
            }
        }

        private async Task<string> GetAsync(string url)
        {
            HttpResponseMessage response = await client.GetAsync(url);
            response.EnsureSuccessStatusCode();
            return await response.Content.ReadAsStringAsync();
        }

        private async Task<string> PostAsync(string url, HttpContent content)
        {
            HttpResponseMessage response = await client.PostAsync(url, content);
            response.EnsureSuccessStatusCode();
            return await response.Content.ReadAsStringAsync();
        }
    }

}

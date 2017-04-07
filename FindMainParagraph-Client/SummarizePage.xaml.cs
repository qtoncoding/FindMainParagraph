using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Imaging;
using Windows.Storage.Pickers;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

using MainParagraphWinRT;
using Windows.UI.Xaml.Media.Imaging;
using System.Threading.Tasks;
using Windows.Media.Ocr;
using Newtonsoft.Json;
using Windows.Storage.Streams;
using Windows.Web.Http;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace FindMainParagraph_Client
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SummarizePage : Page
    {
        private HttpClient httpClient;
        private SoftwareBitmap finalBitmap = null;
        private SoftwareBitmap originalBitmap = null;
        private string originalSummarizedText = null;
        private string finalSummarizedText = null;
        private SoftwareBitmapSource originalSource = null;
        private SoftwareBitmapSource finalSource = null;

        public SummarizePage()
        {
            this.InitializeComponent();
        }

        private async void BrowseButton_Click(object sender, RoutedEventArgs e)
        {
            var picker = new FileOpenPicker()
            {
                SuggestedStartLocation = PickerLocationId.PicturesLibrary
            };
            picker.FileTypeFilter.Add(".jpg");
            picker.FileTypeFilter.Add(".png");
            picker.ViewMode = PickerViewMode.Thumbnail;
            var file = await picker.PickSingleFileAsync();
            if (file != null)
            {
                using (var stream = await file.OpenReadAsync())
                {
                    var decoder = await BitmapDecoder.CreateAsync(stream);
                    originalBitmap = await decoder.GetSoftwareBitmapAsync();
                }
                originalSource = await GetBitmapSourceAsync(originalBitmap);

                var findMainParagraph = new FindMainParagraph(originalBitmap);
                finalBitmap = findMainParagraph.FinalImage();
                finalSource = await GetBitmapSourceAsync(finalBitmap);

                if (ParagraphSwitch.IsOn)
                {
                    Original.Source = finalSource;
                }
                else
                {
                    Original.Source = originalSource;
                }

                BrowseButton.Visibility = Visibility.Collapsed;
            }
        }

        private async Task<SoftwareBitmapSource> GetBitmapSourceAsync(SoftwareBitmap bitmap)
        {
            var imageSource = new SoftwareBitmapSource();
            var newbitmap = SoftwareBitmap.Convert(bitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
            await imageSource.SetBitmapAsync(newbitmap);
            return imageSource;
        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            var ocrEngine = OcrEngine.TryCreateFromUserProfileLanguages();
            var originalResult = await ocrEngine.RecognizeAsync(originalBitmap);
            var finalResult = await ocrEngine.RecognizeAsync(finalBitmap);

            httpClient = new HttpClient();
            originalSummarizedText = await getSummarizedText(originalResult, httpClient);
            finalSummarizedText = await getSummarizedText(finalResult, httpClient);

            if (ParagraphSwitch.IsOn)
            {
                Summarization.Text = finalSummarizedText;
            }
            else
            {
                Summarization.Text = originalSummarizedText;
            }
        }

        private static async Task<string> getSummarizedText(OcrResult originalResult, HttpClient client)
        {
            var requestObject = new RequestObject() { content = originalResult.Text };

            var requestObjectString = JsonConvert.SerializeObject(requestObject);

            var dataWriter = new DataWriter()
            {
                UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8
            };
            dataWriter.WriteString(requestObjectString);
            var buffer = dataWriter.DetachBuffer();

            var messageContent = new HttpBufferContent(buffer);

            messageContent.Headers.ContentType = new Windows.Web.Http.Headers.HttpMediaTypeHeaderValue("application/json");
            var message = new HttpRequestMessage(HttpMethod.Post, ConfigConstants.EndPoint)
            {
                Content = messageContent
            };

            var response = await client.SendRequestAsync(message);
            var responseBuffer = await response.Content.ReadAsBufferAsync();
            var dataReader = DataReader.FromBuffer(responseBuffer);
            dataReader.UnicodeEncoding = Windows.Storage.Streams.UnicodeEncoding.Utf8;
            var responseString = dataReader.ReadString(responseBuffer.Length);
            var responseObject = JsonConvert.DeserializeObject<ResponseObject>(responseString);

            return responseObject.summary;
        }

        private void ParagraphSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (finalSource != null && originalSource != null)
            {
                if (ParagraphSwitch.IsOn)
                {
                    Original.Source = finalSource;
                }
                else
                {
                    Original.Source = originalSource;
                }
            }

            if (finalSummarizedText != null && originalSummarizedText != null)
            {
                if (ParagraphSwitch.IsOn)
                {
                    Summarization.Text = finalSummarizedText;
                }
                else
                {
                    Summarization.Text = originalSummarizedText;
                }
            }

        }
    }
}

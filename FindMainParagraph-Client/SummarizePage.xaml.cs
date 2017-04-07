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

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace FindMainParagraph_Client
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class SummarizePage : Page
    {
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
                SoftwareBitmap originalBitmap;
                using (var stream = await file.OpenReadAsync())
                {
                    var decoder = await BitmapDecoder.CreateAsync(stream);
                    originalBitmap = await decoder.GetSoftwareBitmapAsync();
                }

                if (ParagraphSwitch.IsOn)
                {
                    var findMainParagraph = new FindMainParagraph(originalBitmap);
                    var finalBitmap = findMainParagraph.FinalImage();

                    var finalSource = await GetBitmapSourceAsync(finalBitmap);
                    Original.Source = finalSource;
                }
                else
                {
                    var originalSource = await GetBitmapSourceAsync(originalBitmap);
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

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Do summarize
        }
    }
}

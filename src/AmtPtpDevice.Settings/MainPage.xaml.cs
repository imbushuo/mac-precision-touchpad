using AmtPtpDevice.Settings.DataObjects;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Devices.Enumeration;
using Windows.Devices.HumanInterfaceDevice;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

namespace AmtPtpDevice.Settings
{
    public sealed partial class MainPage : Page
    {
        private HidDevice m_hidDevice;
        private DeviceWatcher m_devWatcher;
        private ConcurrentDictionary<string, DeviceInformation> m_devices;
        private PtpUserModeConfReport m_report;

        public MainPage()
        {
            this.InitializeComponent();
            m_devices = new ConcurrentDictionary<string, DeviceInformation>();

            this.Loaded += OnPageLoaded;
        }

        private void OnPageLoaded(object sender, RoutedEventArgs e)
        {
            SetupWatcher();
        }

        private void SetupWatcher()
        {
            var deviceSelector = HidDevice.GetDeviceSelector(0xff00, 0x0001, 0x05ac, 0x0265);
            
            if (m_devWatcher != null)
            {
                m_devWatcher.Stop();
                m_devices.Clear();
            }

            m_devWatcher = DeviceInformation.CreateWatcher(deviceSelector);

            m_devWatcher.Added += OnDeviceAdded;
            m_devWatcher.Removed += OnDeviceRemoved;
            m_devWatcher.EnumerationCompleted += OnEnumerationCompleted;

            m_devWatcher.Start();
        }

        private async void OnEnumerationCompleted(DeviceWatcher sender, object args)
        {
            if (!m_devices.Any()) return;
            var trackpad = m_devices.First();

            m_hidDevice = await HidDevice.FromIdAsync(trackpad.Key, FileAccessMode.ReadWrite);
            var sReport = await m_hidDevice.GetFeatureReportAsync(0x09);
            var ptr = Marshal.AllocHGlobal((int) sReport.Data.Length);
            Marshal.Copy(sReport.Data.ToArray(), 0, ptr, (int)sReport.Data.Length);

            m_report = Marshal.PtrToStructure<PtpUserModeConfReport>(ptr);
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal,() =>
            {
                // Set value
                m_sensitivitySlider.Value = m_report.PressureQualificationLevel;
                m_confidenceSlider.Value = m_report.SingleContactSizeQualificationLevel;
            });

            Marshal.FreeHGlobal(ptr);
        }

        private void OnDeviceRemoved(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            m_devices.TryRemove(args.Id, out DeviceInformation removedDevice);
        }

        private void OnDeviceAdded(DeviceWatcher sender, DeviceInformation args)
        {
            m_devices.TryAdd(args.Id, args);
        }

        private async void OnSetButtonClicked(object sender, RoutedEventArgs e)
        {
            m_report.PressureQualificationLevel = (byte) m_sensitivitySlider.Value;
            m_report.SingleContactSizeQualificationLevel = (byte) m_confidenceSlider.Value;

            if (m_hidDevice == null) return;
            var featureReport = m_hidDevice.CreateFeatureReport(0x09);
            using (var datawriter = new DataWriter())
            {
                datawriter.WriteByte(0x09);
                datawriter.WriteByte(m_report.PressureQualificationLevel);
                datawriter.WriteByte(m_report.SingleContactSizeQualificationLevel);
                datawriter.WriteByte(m_report.MultipleContactSizeQualificationLevel);
                featureReport.Data = datawriter.DetachBuffer();
                await m_hidDevice.SendFeatureReportAsync(featureReport);
            }
        }
    }
}

using System;
using System.Threading;
using Windows.Devices.Enumeration;
using Windows.Devices.HumanInterfaceDevice;
using Windows.Storage;

namespace AmtPtpDevice.Settings.Comm
{
    public class UsbHidDeviceAccessSubscription : IDisposable
    {

        private string m_deviceId;
        private HidDevice m_device;
        private int m_refCount;
        private DeviceWatcher m_deviceWatcher;
        private EventHandler m_targetDeviceAvailable;

        public string AqsString { get; }
        public HidDevice Device
        {
            get
            {
                return m_device;
            }
        }

        public event EventHandler TargetDeviceAvailable
        {
            add
            {
                lock (m_targetDeviceAvailable)
                {
                    m_targetDeviceAvailable += value;
                }

                Interlocked.Increment(ref m_refCount);
                m_deviceWatcher.Start();
            }
            remove
            {
                Interlocked.Decrement(ref m_refCount);
                if (m_refCount == 0)
                {
                    m_deviceWatcher.Stop();
                }

                lock (m_targetDeviceAvailable)
                {
                    m_targetDeviceAvailable -= value;
                }
            }
        }
        public event EventHandler TargetDeviceLost;

        public UsbHidDeviceAccessSubscription(string aqsString)
        {
            AqsString = aqsString;
            m_device = null;
            m_deviceId = null;
            m_targetDeviceAvailable = new EventHandler((d, a) => { });
            m_deviceWatcher = DeviceInformation.CreateWatcher(aqsString);
            m_deviceWatcher.Added += SbDeviceAdded;
            m_deviceWatcher.Removed += SbDeviceRemoved;
        }

        private void SbDeviceRemoved(DeviceWatcher sender, DeviceInformationUpdate args)
        {
            if (m_deviceId != args.Id) return;

            try
            {
                m_device.Dispose();
            }
            finally
            {
                TargetDeviceLost?.Invoke(this, null);
            }
        }

        private async void SbDeviceAdded(DeviceWatcher sender, DeviceInformation args)
        {
            m_device = await HidDevice.FromIdAsync(args.Id, FileAccessMode.ReadWrite);
            m_deviceId = args.Id;
            m_targetDeviceAvailable?.Invoke(this, null);
        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    m_deviceWatcher.Added -= SbDeviceAdded;
                    m_deviceWatcher.Removed -= SbDeviceRemoved;
                    m_deviceWatcher.Stop();
                    m_device.Dispose();
                }

                m_deviceId = null;
                m_deviceWatcher = null;
                m_device = null;
                disposedValue = true;
            }
        }

        // This code added to correctly implement the disposable pattern.
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            Dispose(true);
        }
        #endregion

    }
}

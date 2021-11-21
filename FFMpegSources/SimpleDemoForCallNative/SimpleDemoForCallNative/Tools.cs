using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace SimpleDemoForCallNative
{
    public class Tools
    {
        // 
        public static string MarshalUtf8String(IntPtr ptr)
        {
            MarshalPtrToUtf8 marshaler = new MarshalPtrToUtf8();
            var obj = marshaler.MarshalNativeToManaged(ptr);
            return obj as string;
        }
    }

    // http://blog.gebhardtcomputing.com/2007/11/marshal-utf8-strings-in-net.html
    public class MarshalPtrToUtf8 : ICustomMarshaler
    {
        private static MarshalPtrToUtf8 _instance = new MarshalPtrToUtf8();

        public MarshalPtrToUtf8()
        {
        }

        public IntPtr MarshalManagedToNative(object ManagedObj)
        {
            if (ManagedObj == null)
                return IntPtr.Zero;
            if (!(ManagedObj is string))
                throw new InvalidOperationException("MarshalManagedToNative:ManagedObj must be string");

            byte[] utf8bytes = Encoding.UTF8.GetBytes(ManagedObj as string);
            IntPtr ptr = Marshal.AllocCoTaskMem(utf8bytes.Length + 1);
            Marshal.Copy(utf8bytes, 0, ptr, utf8bytes.Length);
            Marshal.WriteByte(ptr, utf8bytes.Length, 0);
            return ptr;
        }

        public object MarshalNativeToManaged(IntPtr pNativeData)
        {
            if (pNativeData == IntPtr.Zero)
                return null;
            List<byte> bytes = new List<byte>();
            for (int offset = 0; ; offset++)
            {
                byte b = Marshal.ReadByte(pNativeData, offset);
                if (b == 0)
                    break;
                else bytes.Add(b);
            }

            var str = Encoding.UTF8.GetString(bytes.ToArray(), 0, bytes.Count);
            return str;
        }

        public void CleanUpManagedData(object ManagedObj)
        {
        }

        public void CleanUpNativeData(IntPtr pNativeData)
        {
            Marshal.FreeCoTaskMem(pNativeData);
            pNativeData = IntPtr.Zero;
        }

        public int GetNativeDataSize()
        {
            return -1;
        }

        public static ICustomMarshaler GetInstance(string s)
        {
            return _instance;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;


namespace SimpleDemoForCallNative
{

    // 
    // Lib Video Player Export Interface
    //
    public class LibVideoPlayerExport
    {

        [DllImport("videoPlayerLib", EntryPoint = "enter_test_videoPlayer_static_Lib")]
        public static extern int enter_test_videoPlayer_static_Lib();

        [DllImport("videoPlayerLib")]
        public static extern int player_init();
        [DllImport("videoPlayerLib")]
        public static extern int player_startPlayVideo(string filepath);
        [DllImport("videoPlayerLib")]
        public static extern int player_renderOneFrame();
        [DllImport("videoPlayerLib")]
        public static extern IntPtr player_getOneFrameBuffer();

        // we need to frame buffer data alloc in native C++
        [DllImport("videoPlayerLib")]
        public static extern void player_getOneFrameBuffer_Done(IntPtr data);
        [DllImport("videoPlayerLib")]
        public static extern int player_shutdown();
        [DllImport("videoPlayerLib")]
        public static extern int player_get_width();
        [DllImport("videoPlayerLib")]
        public static extern int player_get_height();
    }

    //
    // Lib Chat Export Interface
    //

    [StructLayout(LayoutKind.Sequential)]
    public struct tools_struct_infor
    {
        // just use one byte
        [MarshalAs(UnmanagedType.I1)] public bool bvalue;
        public int ivalue;
        public int x;
        public int y;
        [MarshalAs(UnmanagedType.LPStr)] public string messagestr;
        [MarshalAs(UnmanagedType.LPStr)] public string namestr;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct tools_struct_person
    {
        // can't calculate native size when using MarshalPtrToUtf8!
        //[MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(MarshalPtrToUtf8))] public string firstName;
        //[MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(MarshalPtrToUtf8))] public string secondName;
        [MarshalAs(UnmanagedType.LPStr)] public string firstName;
        [MarshalAs(UnmanagedType.LPStr)] public string secondName;
        public int age;
        public int num;
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct MyPerson_Info
    {
        public IntPtr person;
        public IntPtr infor;

        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
        public int[] vals;

        public string infortitle;
    }

    // 调用方将清理堆栈 cdecl
    // 被调用方将清理堆栈 StdCall
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void tool_callback_int(int value, string message);

    public class LibChatExport
    {
        [DllImport("chatEngineLib")]
        public static extern void tool_c_strncpy(StringBuilder dest, string src, uint n);

        [DllImport("chatEngineLib")]
        public static extern IntPtr tool_get_one_person(string firstName, string secondName, bool man, int age);

        [DllImport("chatEngineLib")]
        public static extern void tool_modify_person(ref MyPerson_Info infor);

        [DllImport("chatEngineLib")]
        public static extern int tool_get_info(tools_struct_person person);

        [DllImport("chatEngineLib")]
        public static extern IntPtr tool_get_string(string inputstring);

        [DllImport("chatEngineLib")]
        public static extern int tool_get_pow(int a, int b);

        [DllImport("chatEngineLib")]
        public static extern void tool_do_calculate(int a, int b, int caltype, tool_callback_int callback);

        [DllImport("chatEngineLib")]
        public static extern void tool_pass_array(IntPtr meshvertex, int vertexcount, IntPtr pos, IntPtr normal);

        // https://limbioliong.wordpress.com/2011/06/16/returning-strings-from-a-c-api/
        // C++ can't just malloc to return char* 
        // should alloc unmanaged string by 
        // 1. CoTaskMemAlloc()/Marshal.FreeCoTaskMem().
        // 2. SysAllocString/Marshal.FreeBSTR().
        // we need just delegate or reference to pass string char* from C++ to C#
        // https://stackoverflow.com/questions/393509/usage-of-cotaskmemalloc
        // Use CoTaskMemAlloc when returning a char* from a native C++ library to.NET as a string.
        // Since .NET uses CoTaskMemFree, you have to allocate the string like this, 
        // you can't allocate it on the stack or the heap using malloc / new
        [DllImport("chatEngineLib", CharSet = CharSet.Ansi)]
        public static extern IntPtr tool_get_vertex_pos();

        [DllImport("chatEngineLib")]
        public static extern void tool_get_vertex_pos_callback(tool_callback_int callback);
    }

    public class MeshVertex
    {
        public float[] vertices;
        public float[] normals;
    }

    class Program
    {
        static void Main(string[] args)
        {
            TestDLL();

            MeshVertex vertex = new MeshVertex();
            vertex.vertices = new float[12];
            vertex.normals = new float[12];
            int vertexCount = 4;

            GCHandle handle_vertex = GCHandle.Alloc(vertex.vertices, GCHandleType.Pinned);
            GCHandle handle_normal = GCHandle.Alloc(vertex.normals, GCHandleType.Pinned);

            // wrong result here
            // C++ return string to C# 
            // recommend callback or using StringBuilder or passing a String with a Buffer and modify in C++
            LibChatExport.tool_pass_array(IntPtr.Zero, vertexCount, handle_vertex.AddrOfPinnedObject(),
                handle_normal.AddrOfPinnedObject());
            IntPtr message = LibChatExport.tool_get_vertex_pos();
            string strmessage = Marshal.PtrToStringAnsi(message);
            Console.WriteLine("message: " + strmessage);

            LibChatExport.tool_get_vertex_pos_callback(((value, s) =>
            {
                Console.WriteLine("-------------");
                Console.WriteLine("vertex size: " + value);
                Console.WriteLine("vertex info: " + s);
                Console.WriteLine("-------------");
            }));

            handle_vertex.Free();
            handle_normal.Free();

            // 
            // Test for C++ simplevideoplayer
            //

            int ret = LibVideoPlayerExport.player_init();
            Debug.Assert(ret == 0);
            string mediapath =
                "G:/KunDev/nativeunity3dplayer-master/simplevideodemo/FFMpegSources/Resources/small_bunny_1080p_60fps.mp4";
            ret = LibVideoPlayerExport.player_startPlayVideo(mediapath);
            Debug.Assert(ret == 0);


            int videoWidth = LibVideoPlayerExport.player_get_width();
            int videoHeight = LibVideoPlayerExport.player_get_height();

            int buffsize = (int)((videoWidth * videoHeight) * 1.5f);
            int frame = 10;
            while (frame >= 0)
            {
                string path = "G:/KunDev/nativeunity3dplayer-master/simplevideodemo/FFMpegSources/debugoutput";
                string savefilename = string.Format("{0}.originfile", frame);
                savefilename = Path.Combine(path, savefilename);
                LibVideoPlayerExport.player_renderOneFrame();
                IntPtr buf = LibVideoPlayerExport.player_getOneFrameBuffer();
                if (buf == IntPtr.Zero)
                {
                    Console.WriteLine("·······render video frame skip this empty frame!!!!");
                }
                else
                {
                    Console.WriteLine("·······render video frame try save to local file now...");
                    byte[] targetbuf = new byte[buffsize];
                    Marshal.Copy(buf, targetbuf, 0, buffsize);
                    File.WriteAllBytes(savefilename, targetbuf);

                    // split to YUV buffer
                    // 1. Y buffer
                    // 2. U buffer
                    // 3. V buffer

                    int ybufsize = videoHeight * videoWidth;
                    int ubufsize = videoHeight * videoWidth / 4;
                    int vbufsize = videoHeight * videoWidth / 4;

                    byte[] ybuff = new byte[ybufsize];
                    Buffer.BlockCopy(targetbuf, 0, ybuff, 0, ybufsize);
                    SaveFileToLocal(ybuff, $"{frame}-y.file", path);

                    byte[] ubuff = new byte[ubufsize];
                    Buffer.BlockCopy(targetbuf, ybufsize, ubuff, 0, ubufsize);
                    SaveFileToLocal(ubuff, $"{frame}-u.file", path);

                    byte[] vbuff = new byte[vbufsize];
                    Buffer.BlockCopy(targetbuf, ybufsize + ubufsize, vbuff, 0, vbufsize);
                    SaveFileToLocal(vbuff, $"{frame}-v.file", path);
                }
                LibVideoPlayerExport.player_getOneFrameBuffer_Done(buf);
                frame--;
            }

            ret = LibVideoPlayerExport.player_shutdown();
            Debug.Assert(ret == 0);
        }

        private static void SaveFileToLocal(byte[] bytes, string filename, string folderpath)
        {
            string path = Path.Combine(folderpath, filename);
            File.WriteAllBytes(path, bytes);
        }

        public static void TestDLL()
        {
            tools_struct_infor infor = new tools_struct_infor();
            infor.bvalue = false;
            infor.ivalue = 1000000;
            infor.x = 1111;
            infor.y = 2222;
            infor.messagestr = "Jack";
            infor.namestr = "Ma";
            infor.x = 10;
            infor.y = 100;

            tools_struct_person person = new tools_struct_person();
            person.age = 10;
            person.firstName = "Fun";
            person.secondName = "Shit";
            person.num = 10;

            int size = Marshal.SizeOf(infor);
            Console.WriteLine("size: " + size);
            IntPtr ptrInforBuffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(infor));
            Marshal.StructureToPtr(infor, ptrInforBuffer, false);

            IntPtr ptrPersonBuffer = Marshal.AllocCoTaskMem(Marshal.SizeOf(person));
            Marshal.StructureToPtr(person, ptrPersonBuffer, false);

            MyPerson_Info personInfo = new MyPerson_Info();
            personInfo.infor = ptrInforBuffer;
            personInfo.person = ptrPersonBuffer;
            personInfo.infortitle = "My MAN";

            Console.WriteLine(
                $"before--->name: {person.firstName}\nsecondname: {person.secondName}\nage: {person.age}");
            Console.WriteLine(
                $"before--->message: {infor.messagestr}\nbvalue: {infor.bvalue}\nintvalue: {infor.ivalue}\nx: {infor.x}\ny: {infor.y}");

            LibChatExport.tool_modify_person(ref personInfo);

            tools_struct_person personaftermodify =
                (tools_struct_person)Marshal.PtrToStructure(personInfo.person, typeof(tools_struct_person));
            infor = (tools_struct_infor)Marshal.PtrToStructure(personInfo.infor, typeof(tools_struct_infor));
            Console.WriteLine(
                $"\nafter--->name: {personaftermodify.firstName}\nsecondname: {personaftermodify.secondName}\nage: {personaftermodify.age}");
            Console.WriteLine(
                $"after--->message: {infor.messagestr}\nbvalue: {infor.bvalue}\nintvalue: {infor.ivalue}\nx: {infor.x}\ny: {infor.y}");
            Console.WriteLine($"after---> array:\n");
            foreach (var value in personInfo.vals)
            {
                Console.WriteLine(value);
            }

            Marshal.FreeCoTaskMem(ptrInforBuffer);
            Marshal.FreeCoTaskMem(ptrPersonBuffer);

            IntPtr retbuf = LibChatExport.tool_get_string("Hello world");
            string retstr = Marshal.PtrToStringAnsi(retbuf);
            Console.WriteLine("get string: " + retstr);

            int ret = LibChatExport.tool_get_pow(2, 4);
            Console.WriteLine("pow ret: " + ret);
            Console.WriteLine("--------------------");
            IntPtr newperson = LibChatExport.tool_get_one_person("Jack", "Chen", false, 20);
            tools_struct_person toolsperson = (tools_struct_person)Marshal.PtrToStructure(newperson, typeof(tools_struct_person));
            printPerson(toolsperson);
            Console.WriteLine("--------------------");
            LibChatExport.tool_do_calculate(100, 200, 1, (value, message) =>
            {
                Console.WriteLine("value: " + value);
                Console.WriteLine("message: " + message);
            });
        }

        private static void printPerson(tools_struct_person person)
        {
            Console.WriteLine("first name: " + person.firstName);
            Console.WriteLine("second name: " + person.secondName);
            Console.WriteLine("age: " + person.age);
            Console.WriteLine("num: " + person.num);
        }
    }
}

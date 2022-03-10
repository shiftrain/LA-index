    QueryPerformanceCounter(&t21);
    double time1 = (double)(t21.QuadPart - t11.QuadPart) / (double)tc1.QuadPart;
    cout << "time = " << time1 << "s" << endl; //输出时间（单位：
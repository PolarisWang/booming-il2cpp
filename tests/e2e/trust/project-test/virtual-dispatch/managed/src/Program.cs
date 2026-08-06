// Entry point for VirtualDispatch test project
using System;
using VirtualDispatch;

var circle = new Circle(5.0);
var rect = new Rectangle(3.0, 4.0);

// Virtual calls through base reference
Console.WriteLine("RTS_VIRT_CIRCLE=" + DispatchSubjects.TestVirtualCall(circle));
Console.WriteLine("RTS_VIRT_RECT=" + DispatchSubjects.TestVirtualCall(rect));

// Interface calls through interface reference
double circleArea = DispatchSubjects.TestInterfaceCall(circle);
double rectArea = DispatchSubjects.TestInterfaceCall(rect);
Console.WriteLine("RTS_IFACE_CIRCLE_AREA=" + (int)circleArea);
Console.WriteLine("RTS_IFACE_RECT_AREA=" + (int)rectArea);

// Derived interface call
Console.WriteLine("RTS_IFACE_COLOR=" + DispatchSubjects.TestDerivedInterfaceCall(circle));

// Direct interface dispatch on Circle object
Console.WriteLine("RTS_IFACE_GETNAME=" + ((IShape)circle).GetName());

Console.WriteLine("RTS_DONE=ok");

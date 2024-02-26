#////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
# Name        :
# Author      : Avi
# Revision    : $Revision: #10 $
#
# Copyright 2009- ECMWF.
# This software is licensed under the terms of the Apache Licence version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
# Checking Job Creation
#for the record, Emos alternative syntax is,

#!/usr/bin/env python2.7
import os
from ecf import *
print "suite creation"
defs = Defs()
defs.suite("test").add(
    Variables(ECF_HOME= os.path.join(os.getenv("HOME"), "course")),
    Task("t1"))
print defs
print "Checking job creation: .ecf -> .job0"  
print defs.check_job_creation()
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# Load the file
#Using "if __name__ == '__main__':" the script can be used both as a module (providing a function) or as a standalone script:

#!/usr/bin/env python2.7
import os
from ecf import *

def generate():
    # print "Creating suite definition"  
    return Defs().add(
        Suite("test").add(
            Variables(ECF_HOME= os.path.join(os.getenv("HOME"),  "course")),
            Task("t1")))
 
if __name__ == '__main__':
    defs = generate()      
    print defs
    print "Checking job creation: .ecf -> .job0"  
    print defs.check_job_creation()
    print "Saving definition to file 'test.def'"
    defs.save_as_defs("test.def")
    try:
        print "Load the in memory definition(defs) into the server"
        Client("localhost@%s" % os.getenv("ECF_PORT")).load(defs)
    except RuntimeError as e: print "Failed:", e
        
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# families
#!/usr/bin/env python2.7
import os
from ecf import *
print "Creating suite definition"  
defs = Defs().add(Suite("test").add(
    Variable("ECF_HOME", os.path.join(os.getenv("HOME"),  "course")),
    Family("f1").add(
      Task("t1"),
      Task("t2"))))
print defs
print "Checking job creation: .ecf -> .job0"  
print defs.check_job_creation()
print "Saving definition to file 'test.def'"
defs.save_as_defs("test.def")
print "Client -> Server: delete, then load a new definition"  
try:
    ci = Client()
    ci.replace("/test", "test.def") # replace the suite into the server
    ci.begin_suite("test")    # start the suite
except RuntimeError as e:
    print "Failed:",   e

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# ecflow variables
#!/usr/bin/env python2.7
import os
from ecf import *
home = os.path.join(os.getenv("HOME"), "course")
 
def create_family_f1():
    return Family("f1" ).add(
        Task("t1").add(Variables(SLEEP= 20)),
        Task("t2").add(Variables(SLEEP= 20)))
 
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    create_family_f1() ))
 
print defs
defs.save_as_defs("test.def")

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# variable inheritance
# tab constraint is relaxed with Emos ecf.py
#!/usr/bin/env python2.7
import os
from ecf import *
home = os.path.join(os.getenv("HOME"), "course")
def create_family_f1():
    return Family("f1" ).add(
        Variables(SLEEP= 20),
        Task("t1"),
        Task("t2"))
print "Creating suite definition"
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
             ECF_HOME=    home),
     create_family_f1() ))
print defs

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# triggers#!/usr/bin/env python2.7
import os
from ecf import *
home=  os.path.join(os.getenv("HOME"), "course")
def create_family_f1():
    return Family("f1" ).add(
        Variable(SLEEP= 20),
        Task("t1"),
        Task("t2").add(Trigger("t1 eq complete")))
print "Creating suite definition"
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    create_family_f1() ))
print defs
defs.save_as_defs("test.def")

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# add an event
#!/usr/bin/env python2.7
import os
from ecf import *
home = os.path.join(os.getenv("HOME"), "course")
def create_family_f1():
    return Family("f1").add(
        Variables(SLEEP= 20),
        Task("t1"),
        Task("t2").add(
            Trigger(["t1"]),
            Event("a"),
            Event("b")),
     
        Task("t3").add(Trigger("t2:a")),
        Task("t4").add(Trigger("t2:b")),
    )
print "Creating suite definition"
defs = Defs().add(
    Suite("test").add(
        Variables(ECF_INCLUDE= home,
                  ECF_HOME=    home),
        create_family_f1() ))
print defs
defs.save_as_defs("test.def")

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# add a complete
#!/usr/bin/env python2.7
import os
from ecf import *
home = os.path.join(os.getenv("HOME"),  "course")
def create_family_f1():
    return Family("f1").add(
        Variable(SLEEP= 20),
        Task("t1"),
        Task("t2").add(Trigger("t1 eq complete"),
                       Event("a"),
                       Event("b")),
        Task("t3").add(Trigger("t2:a")),
        Task("t4").add(Trigger("t2 eq complete"), 
                       Complete("t2:b")  ))
       
print "Creating suite definition"  
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    create_family_f1() ))
print defs
defs.save_as_defs("test.def")

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# add a meter
#!/usr/bin/env python2.7
import os
from ecf import *
home= os.path.join(os.getenv("HOME"), "course")
def create_family_f1():
    return Family("f1").add(
        Variables(SLEEP= 20),
        Task("t1").add(Meter("progress", 1, 100, 90)),
     
        Task("t2").add(Trigger("t1 eq complete"),
                       Event("a"),
                       Event("b")),
     
        Task("t3").add(Trigger("t2:a")),
     
        Task("t4").add(Trigger("t2 eq complete"),
                       Complete("t2:b")),
      
        Task("t5").add(Trigger("t1:progress ge 30")),
        Task("t6").add(Trigger("t1:progress ge 60")),
        Task("t7").add(Trigger("t1:progress ge 90")))
    
print "Creating suite definition"  
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    create_family_f1() ))
print defs
defs.save_as_defs("test.def")


>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# Time dependencies
# #!/usr/bin/env python2.7
import os

home = os.path.join(os.getenv("HOME"),  "course")
 
def create_family_f2():
    return Family("f2").add(
        Variables(SLEEP= 20),
        Task("t1").add(
          Time("00:30 23:30 00:30")), # start(hh:mm) end(hh:mm) increment(hh:mm)
        Task("t2").add(Day( "sunday" )),
        Task("t3").add(
            Date("1.*.*"),  # day month year, first of every month or every year
            Time("12:00")), # hour, minutes at 12 o'clock
    
        Task("t4").add(Time("+00:02")), # hour, minutes, relative to suite start
                                        # 2 minutes after family f2 start
        Task("t5").add(Time("00:02"))   # hour, minutes suite site
                                        # 2 minutes past midnight
)           
    
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    create_family_f1(),
    create_family_f2(), ))
print defs
defs.save_as_defs("test.def")


>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# indentation
# Another approach is to use ecf.py module to benefit from object oriented polymorphism
#!/usr/bin/env python2.7
import os
import sys
sys.path.append('/home/ma/emos/def/o/def')
from ecf import *
print "Creating suite definition" 
defs = Defs().add(# Stream like definition
    Suite("test").add(
        Variables({ # a dictionary to detect duplicated variables
                "ECF_INCLUDE": os.getenv("HOME") + "/course",
                "ECF_HOME":    os.getenv("HOME") + "/course",}),
        Family("f1").add(
            Variable("SLEEP", "20"),
            Task("t1").add(Meter("progress", 1, 100, 90)),
            Task("t2").add(
                Trigger("t1 eq complete"),
                Event("a"),
                Event("b")),
            Task("t3").add(Trigger("t2:a")),
            Task("t4").add(Trigger("t2 eq complete"),
                           Complete("t2:b")),
            Task("t5").add(Trigger("t1:progress ge 30")),
            Task("t6").add(Trigger("t1:progress ge 60")),
            Task("t7").add(Trigger("t1:progress ge 90")),),
        Family("f2").add(
            Variable("SLEEP", "20"),
            Task("t1").add(Time( "00:30 23:30 00:30" )),
            Task("t2").add(Day( "sunday" )),
            Task("t3").add(Date("1.*.*"),
                           Time("12:00")),
            Task("t4").add(Time("+00:02")),
            Task("t5").add(Time("00:02")))))
             
out = file("test.def", "w")
print >>out, defs


>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# label
#!/usr/bin/env python2.7
import os
from ecf import *
home = os.getenv("HOME") + "/course"
print "Creating suite definition"  
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    Family("f3").add(
        Task("t1").add(
            Label("info","")))))
print defs
defs.save_as_defs("test.def")


>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
# repeat
#!/usr/bin/env python2.7
import os
from ecf import *
home = os.path.join(os.getenv("HOME"),  "course")
def create_family_f4():
    return Family("f4").add(
        Variables(SLEEP= 2),
        Repeat("NAME", ["a", "b", "c", "d", "e", "f" ], kind="enum"),
    
        Family("f5").add(
            Repeat( "VALUE", 1, 10, kind= "integer"),
            Task("t1").add(
                Repeat( "DATE", 20101230, 20110105, kind= "date"),
                Label("info", ""))))
     
print "Creating suite definition"  
defs = ecflow.Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    create_family_f4(), ))
print defs
defs.save_as_defs("test.def")

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# limits
#!/usr/bin/env python2.7
import os
from ecf import *
home= os.path.join(os.getenv("HOME"),  "course")
def create_family_f5() :
    return Family("f5").add(
        Inlimit("l1"),
        Variables(SLEEP= 20),
        [ Task( "t" + str(i))     for i in range(1, 10)] # list comprehension
    )
     
print "Creating suite definition"  
defs = Defs().add(Suite("test").add(
    Variables(ECF_INCLUDE= home,
              ECF_HOME=    home),
    Limit("l1", 2),
    create_family_f5() ))
print defs
defs.save_as_defs("test.def")

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#  Data acquiation solution
#!/usr/bin/env python2.7
import os
from ecf import * 
home = os.getenv("HOME") + "/course")  
defs = Defs().add(Suite("data_aquisition").add(
    Repeat(kind="day", step=1),
    Variables(ECF_HOME= home,
              ECF_INCLUDE= home,
              ECF_FILES= home + "/data",
              SLEEP= 2),
    [ Family(city).add(
        Task("archive").add(
            [ Family(obs_type).add(
                If( city in ("Exeter", "Toulouse", "Offenbach"),
                    Time("00:00 23:00 01:00")),
                If( city in ("Washington"),
                    Time("00:00 23:00 03:00")),
                If( city in ("Tokyo"),
                    Time("12:00")),
                If( city in ("Melbourne"),
                    Day( "monday" )),
                If( city in ("Montreal"),
                    Date("1.*.*")),
          
                Task("get"),
                Task("process").add(Trigger("get eq complete")),
                Task("store").add(Trigger("get eq complete")))
              for obs_type in ( "observations", "fields", "images" ) ] ))
        for city in ( "Exeter", "Toulouse", "Offenbach", "Washington", "Tokyo", "Melbourne", "Montreal" ) ]
    ))
print defs

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# operational suite solution:

#!/usr/bin/env python2.7
import os
from ecf import *
home = os.getenv("HOME") + "/course"
cycle_triggers = None
last_step = { "12": 240,
              "00": 24, }
def cycle_trigger(cycle):
    if cycle == "12": return Trigger("./00==complete")
    return None
defs = Defs().add(Suite("operation_suite").add(
     Repeat(kind="day", step=1),
     Variables(ECF_HOME= home,
               ECF_INCLUDE= home,
               ECF_FILES= home + "/oper"),
    [ Family(str(cycle)).add(
        Variables(CYCLE=  cycle,
                  LAST_STEP= last_step[cycle]),
       
        cycle_trigger(cycle),
                  
        Family("analysis").add(
            Task("get_observations"),
            Task("run_analysis").add(Trigger(["get_observations", ])),
            Task("post_processing").add(Trigger(["run_analysis", ])),
        ),
  
        Family("forecast").add(
            Trigger("analysis == complete"),
            Task("get_input_data"),
            Task("run_forecast").add(
                Trigger(["get_input_data", ]),
                Meter("step", 0, last_step[cycle])),
        ),
        Family("archive").add(
            Family("analysis").add(
                Variables(TYPE= "analysis",
                          STEP= 0),
                Trigger(["../analysis/run_analysis", ]),
                Task("save"),
                [ Family("step_%02d" % i).add(
                    Variables(TYPE= "forecast",
                              STEP= i),
                    Trigger("../../forecast/run_forecast:step ge %d" % i),
                    Task("save"))
                  for i in range(6, last_step+1, 6) ]
            )
        )
    ) for cycle in ( 0 , 12 ) ] ))
print defs

!!!!!!! 
Baudouin Raoult
range(6, last_step, 6) will not return last_step.

>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

# back archiving solution
#!/usr/bin/env python2.7
import os
from ecf import *
home= os.getenv("HOME") + "/course"
defs = Defs().add(Suite("back_archiving").add(
    Repeat( kind= "day", step= 1),
    Variables(ECF_HOME= home,
              ECF_INCLUDE= home,
              ECF_FILES= home + "/back",
              SLEEP= 2),
    Limit("access", 2),
    [ Family(kind).add(
        Repeat( "DATE", 19900101, 19950712, kind="date"),
        Variables(KIND= kind),
        Task("get_old").add(Inlimit("access")),
        Task("convert").add(Trigger("get_old == complete")),
        Task("save_new").add(Trigger("convert == complete")))
      for kind in ( "analysis", "forecast", "climatology", "observations", "images" ) ]
))
print defs

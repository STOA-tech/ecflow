.. index::
   single: trigger (tutorial)

.. _tutorial-add-trigger:  
   
Add Trigger
===========

In the previous exercise we saw that the two tasks were running simultaneously. We would like now to make sure that **t2** only runs once **t1** is complete. For this we have to define a :term:`trigger`

Triggers are used to declare :term:`dependencies` between two tasks. For instance, the second task might need data created by the first task. When ecFlow tries to start a task, it evaluates the :term:`trigger` expression.  If the condition is correct, the task is started, otherwise the task stays :term:`queued`.

Triggers can be between tasks, or between families, or a mixture. Remember the two rules:

* A family is :term:`complete` when all its tasks are :term:`complete`.
* A task will be started if its triggers and the triggers of all is 
  parent families evaluate to true. 

A :term:`node` can only have one trigger expression, but very complex expressions can be built (and keep in mind that the triggers of the parent nodes are also implicit triggers).

Sometimes triggers are also used to prevent too many jobs from running at the same time. In this case the use of a :term:`limit` may be a better solution (we will cover limits later on in the :ref:`tutorial-limits` section).

Nodes can be addressed in trigger expressions using full names: **/test/f1/t1** refers to the :term:`task` **t1**, and **/test/f1** refers to the :term:`family` **f1**. In some contexts, ecFlow will accept relative names, such as **../t1**.

.. code-block:: shell

   trigger /test/f1/t1 == complete

Triggers can be very complex, and ecFlow supports all kinds of conditions (not, and, or, ...), in addition they can also reference Node attributes like :term:`event`, :term:`meter`, :term:`variable`, :term:`repeat` and generated variables.
  
Text
----

.. code-block:: shell

   # Definition of the suite test.
   suite test
      edit ECF_INCLUDE "$HOME/course"   # replace '$HOME' with the path to your home directory
      edit ECF_HOME    "$HOME/course"
      family f1
        edit SLEEP 20
        task t1
        task t2
            trigger t1 eq complete 
      endfamily
   endsuite

Python
------

The trigger expression can be checked, this is **especially** important when dealing with very large suites and **complex** triggers.

.. literalinclude:: src/add-trigger.py
   :language: python
   :caption: $HOME/course/test.py
  

**What to do**

#. Edit the :term:`suite definition` file to add the :term:`trigger`.
#. Replace the :term:`suite`. 
 
   | Python: ``python3 test.py ; python3 client.py``
   | Text: ``ecflow_client --suspend=/test ;  ecflow_client --replace=/test test.def``

#. Observe the tasks in :term:`ecflow_ui`.
#. See the triggers by selecting **t1** or **t2**.
#. See the trigger relation by clicking on the Trigger tab.
#. Search any reference to **t1** by using the search menu.
#. Introduce an error in the trigger expression and ensure that this error is trapped. i.e. change the trigger to.

   .. code-block:: shell

      Trigger("t == complete")  # there is no node with name t, this should be reported as an error



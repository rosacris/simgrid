# 
#  * $Id$
#  *
#  * Copyright 2010 Martin Quinson, Mehdi Fekari           
#  * All right reserved. 
#  *
#  * This program is free software; you can redistribute 
#  * it and/or modify it under the terms of the license 
#  *(GNU LGPL) which comes with this package. 
require 'RubyProcess'
require 'RubyHost'
class ProcessFactory 

#     Attributes
   attr_accessor :args, :proprieties, :hostName, :function
#    Initlialize
    def initialize()
    
    @args = Array.new
    @proprieties = Hash.new
    @hostName = nil
    @function = nil
    
    end
    
#     setProcessIdentity
    def setProcessIdentity(hostName,function)
      @hostName = hostName
      @function = function
      
      if !args.empty?
	args.clear
      end
      
      if !proprieties.empty?
	proprieties.clear   
      end
    
    end

#     RegisterProcess  
    def registerProcessArg(arg)
      
      @args.push(arg)
      
    end

#     CreateProcess
    def createProcess()
      
      process = rubyNewInstance(@function) #process = rubyNewInstanceArgs(@function,@args) #
      size = @args.size
      for i in 0..size-1
 	process.pargs.push(@args[i]) 
      end
      process.name = @function
      process.id = process.nextId() # This increment Automaticly  The Static ProcessNextId for The Class RbProces
      host = RbHost.getByName(@hostName)
      processCreate(process,host)
      process.proprieties = @proprieties
      @proprieties = Hash.new
      
    end
    
#     SetProperty
    def setProperty(id,value)
      @proprieties[id] = value
    end
    
#     End Class
  end
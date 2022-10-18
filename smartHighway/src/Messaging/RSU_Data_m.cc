//
// Generated file, do not edit! Created by nedtool 5.7 from Messaging/RSU_Data.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "RSU_Data_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

VehicleData::VehicleData()
{
    this->speed = 0;
    for (unsigned int i=0; i<2; i++)
        this->position[i] = 0;
}

void __doPacking(omnetpp::cCommBuffer *b, const VehicleData& a)
{
    doParsimPacking(b,a.vehicleId);
    doParsimPacking(b,a.speed);
    doParsimArrayPacking(b,a.position,2);
}

void __doUnpacking(omnetpp::cCommBuffer *b, VehicleData& a)
{
    doParsimUnpacking(b,a.vehicleId);
    doParsimUnpacking(b,a.speed);
    doParsimArrayUnpacking(b,a.position,2);
}

class VehicleDataDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    VehicleDataDescriptor();
    virtual ~VehicleDataDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(VehicleDataDescriptor)

VehicleDataDescriptor::VehicleDataDescriptor() : omnetpp::cClassDescriptor("VehicleData", "")
{
    propertynames = nullptr;
}

VehicleDataDescriptor::~VehicleDataDescriptor()
{
    delete[] propertynames;
}

bool VehicleDataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<VehicleData *>(obj)!=nullptr;
}

const char **VehicleDataDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *VehicleDataDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int VehicleDataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int VehicleDataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *VehicleDataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "vehicleId",
        "speed",
        "position",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int VehicleDataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleId")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "speed")==0) return base+1;
    if (fieldName[0]=='p' && strcmp(fieldName, "position")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *VehicleDataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "double",
        "double",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **VehicleDataDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *VehicleDataDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int VehicleDataDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    VehicleData *pp = (VehicleData *)object; (void)pp;
    switch (field) {
        case 2: return 2;
        default: return 0;
    }
}

const char *VehicleDataDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    VehicleData *pp = (VehicleData *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string VehicleDataDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    VehicleData *pp = (VehicleData *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->vehicleId);
        case 1: return double2string(pp->speed);
        case 2: if (i>=2) return "";
                return double2string(pp->position[i]);
        default: return "";
    }
}

bool VehicleDataDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    VehicleData *pp = (VehicleData *)object; (void)pp;
    switch (field) {
        case 0: pp->vehicleId = (value); return true;
        case 1: pp->speed = string2double(value); return true;
        case 2: if (i>=2) return false;
                pp->position[i] = string2double(value); return true;
        default: return false;
    }
}

const char *VehicleDataDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *VehicleDataDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    VehicleData *pp = (VehicleData *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

Register_Class(RSU_Data)

RSU_Data::RSU_Data(const char *name, short kind) : ::omnetpp::cMessage(name,kind)
{
    this->lastStepOccupancy = 0;
    this->lastStepMeanSpeed = 0;
    this->lastStepHaltingVehiclesNumber = 0;
    vehicles_arraysize = 0;
    this->vehicles = 0;
}

RSU_Data::RSU_Data(const RSU_Data& other) : ::omnetpp::cMessage(other)
{
    vehicles_arraysize = 0;
    this->vehicles = 0;
    copy(other);
}

RSU_Data::~RSU_Data()
{
    delete [] this->vehicles;
}

RSU_Data& RSU_Data::operator=(const RSU_Data& other)
{
    if (this==&other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void RSU_Data::copy(const RSU_Data& other)
{
    this->rsuId = other.rsuId;
    this->lastStepOccupancy = other.lastStepOccupancy;
    this->lastStepMeanSpeed = other.lastStepMeanSpeed;
    this->lastStepHaltingVehiclesNumber = other.lastStepHaltingVehiclesNumber;
    delete [] this->vehicles;
    this->vehicles = (other.vehicles_arraysize==0) ? nullptr : new VehicleData[other.vehicles_arraysize];
    vehicles_arraysize = other.vehicles_arraysize;
    for (unsigned int i=0; i<vehicles_arraysize; i++)
        this->vehicles[i] = other.vehicles[i];
}

void RSU_Data::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->rsuId);
    doParsimPacking(b,this->lastStepOccupancy);
    doParsimPacking(b,this->lastStepMeanSpeed);
    doParsimPacking(b,this->lastStepHaltingVehiclesNumber);
    b->pack(vehicles_arraysize);
    doParsimArrayPacking(b,this->vehicles,vehicles_arraysize);
}

void RSU_Data::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->rsuId);
    doParsimUnpacking(b,this->lastStepOccupancy);
    doParsimUnpacking(b,this->lastStepMeanSpeed);
    doParsimUnpacking(b,this->lastStepHaltingVehiclesNumber);
    delete [] this->vehicles;
    b->unpack(vehicles_arraysize);
    if (vehicles_arraysize==0) {
        this->vehicles = 0;
    } else {
        this->vehicles = new VehicleData[vehicles_arraysize];
        doParsimArrayUnpacking(b,this->vehicles,vehicles_arraysize);
    }
}

const char * RSU_Data::getRsuId() const
{
    return this->rsuId.c_str();
}

void RSU_Data::setRsuId(const char * rsuId)
{
    this->rsuId = rsuId;
}

int RSU_Data::getLastStepOccupancy() const
{
    return this->lastStepOccupancy;
}

void RSU_Data::setLastStepOccupancy(int lastStepOccupancy)
{
    this->lastStepOccupancy = lastStepOccupancy;
}

double RSU_Data::getLastStepMeanSpeed() const
{
    return this->lastStepMeanSpeed;
}

void RSU_Data::setLastStepMeanSpeed(double lastStepMeanSpeed)
{
    this->lastStepMeanSpeed = lastStepMeanSpeed;
}

int RSU_Data::getLastStepHaltingVehiclesNumber() const
{
    return this->lastStepHaltingVehiclesNumber;
}

void RSU_Data::setLastStepHaltingVehiclesNumber(int lastStepHaltingVehiclesNumber)
{
    this->lastStepHaltingVehiclesNumber = lastStepHaltingVehiclesNumber;
}

void RSU_Data::setVehiclesArraySize(unsigned int size)
{
    VehicleData *vehicles2 = (size==0) ? nullptr : new VehicleData[size];
    unsigned int sz = vehicles_arraysize < size ? vehicles_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        vehicles2[i] = this->vehicles[i];
    vehicles_arraysize = size;
    delete [] this->vehicles;
    this->vehicles = vehicles2;
}

unsigned int RSU_Data::getVehiclesArraySize() const
{
    return vehicles_arraysize;
}

VehicleData& RSU_Data::getVehicles(unsigned int k)
{
    if (k>=vehicles_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", vehicles_arraysize, k);
    return this->vehicles[k];
}

void RSU_Data::setVehicles(unsigned int k, const VehicleData& vehicles)
{
    if (k>=vehicles_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", vehicles_arraysize, k);
    this->vehicles[k] = vehicles;
}

class RSU_DataDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    RSU_DataDescriptor();
    virtual ~RSU_DataDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(RSU_DataDescriptor)

RSU_DataDescriptor::RSU_DataDescriptor() : omnetpp::cClassDescriptor("RSU_Data", "omnetpp::cMessage")
{
    propertynames = nullptr;
}

RSU_DataDescriptor::~RSU_DataDescriptor()
{
    delete[] propertynames;
}

bool RSU_DataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RSU_Data *>(obj)!=nullptr;
}

const char **RSU_DataDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *RSU_DataDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int RSU_DataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 5+basedesc->getFieldCount() : 5;
}

unsigned int RSU_DataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISCOMPOUND,
    };
    return (field>=0 && field<5) ? fieldTypeFlags[field] : 0;
}

const char *RSU_DataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "rsuId",
        "lastStepOccupancy",
        "lastStepMeanSpeed",
        "lastStepHaltingVehiclesNumber",
        "vehicles",
    };
    return (field>=0 && field<5) ? fieldNames[field] : nullptr;
}

int RSU_DataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='r' && strcmp(fieldName, "rsuId")==0) return base+0;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastStepOccupancy")==0) return base+1;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastStepMeanSpeed")==0) return base+2;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastStepHaltingVehiclesNumber")==0) return base+3;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicles")==0) return base+4;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *RSU_DataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "int",
        "double",
        "int",
        "VehicleData",
    };
    return (field>=0 && field<5) ? fieldTypeStrings[field] : nullptr;
}

const char **RSU_DataDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RSU_DataDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RSU_DataDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    RSU_Data *pp = (RSU_Data *)object; (void)pp;
    switch (field) {
        case 4: return pp->getVehiclesArraySize();
        default: return 0;
    }
}

const char *RSU_DataDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RSU_Data *pp = (RSU_Data *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RSU_DataDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    RSU_Data *pp = (RSU_Data *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getRsuId());
        case 1: return long2string(pp->getLastStepOccupancy());
        case 2: return double2string(pp->getLastStepMeanSpeed());
        case 3: return long2string(pp->getLastStepHaltingVehiclesNumber());
        case 4: {std::stringstream out; out << pp->getVehicles(i); return out.str();}
        default: return "";
    }
}

bool RSU_DataDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    RSU_Data *pp = (RSU_Data *)object; (void)pp;
    switch (field) {
        case 0: pp->setRsuId((value)); return true;
        case 1: pp->setLastStepOccupancy(string2long(value)); return true;
        case 2: pp->setLastStepMeanSpeed(string2double(value)); return true;
        case 3: pp->setLastStepHaltingVehiclesNumber(string2long(value)); return true;
        default: return false;
    }
}

const char *RSU_DataDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 4: return omnetpp::opp_typename(typeid(VehicleData));
        default: return nullptr;
    };
}

void *RSU_DataDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    RSU_Data *pp = (RSU_Data *)object; (void)pp;
    switch (field) {
        case 4: return (void *)(&pp->getVehicles(i)); break;
        default: return nullptr;
    }
}



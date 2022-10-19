//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Messaging/RSU_Data.msg.
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
#include <memory>
#include <type_traits>
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
    for (int i = 0; i < n; i++) {
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
    for (int i = 0; i < n; i++) {
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
    for (int i = 0; i < n; i++) {
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

VehicleData::VehicleData()
{
    for (size_t i = 0; i < 2; i++)
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
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_vehicleId,
        FIELD_speed,
        FIELD_position,
    };
  public:
    VehicleDataDescriptor();
    virtual ~VehicleDataDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(VehicleDataDescriptor)

VehicleDataDescriptor::VehicleDataDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(VehicleData)), "")
{
    propertyNames = nullptr;
}

VehicleDataDescriptor::~VehicleDataDescriptor()
{
    delete[] propertyNames;
}

bool VehicleDataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<VehicleData *>(obj)!=nullptr;
}

const char **VehicleDataDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *VehicleDataDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int VehicleDataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int VehicleDataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_vehicleId
        FD_ISEDITABLE,    // FIELD_speed
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_position
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *VehicleDataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "vehicleId",
        "speed",
        "position",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int VehicleDataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "vehicleId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "speed") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "position") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *VehicleDataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_vehicleId
        "double",    // FIELD_speed
        "double",    // FIELD_position
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **VehicleDataDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *VehicleDataDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int VehicleDataDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        case FIELD_position: return 2;
        default: return 0;
    }
}

void VehicleDataDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'VehicleData'", field);
    }
}

const char *VehicleDataDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string VehicleDataDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        case FIELD_vehicleId: return oppstring2string(pp->vehicleId);
        case FIELD_speed: return double2string(pp->speed);
        case FIELD_position: if (i >= 2) return "";
                return double2string(pp->position[i]);
        default: return "";
    }
}

void VehicleDataDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        case FIELD_vehicleId: pp->vehicleId = (value); break;
        case FIELD_speed: pp->speed = string2double(value); break;
        case FIELD_position: if (i < 0 || i >= 2) throw omnetpp::cRuntimeError("Array index %d out of bounds for field %d of class 'VehicleData'", i, field);
                pp->position[i] = string2double(value); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'VehicleData'", field);
    }
}

omnetpp::cValue VehicleDataDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        case FIELD_vehicleId: return pp->vehicleId;
        case FIELD_speed: return pp->speed;
        case FIELD_position: if (i >= 2) return omnetpp::cValue();
                return pp->position[i];
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'VehicleData' as cValue -- field index out of range?", field);
    }
}

void VehicleDataDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        case FIELD_vehicleId: pp->vehicleId = value.stringValue(); break;
        case FIELD_speed: pp->speed = value.doubleValue(); break;
        case FIELD_position: if (i < 0 || i >= 2) throw omnetpp::cRuntimeError("Array index %d out of bounds for field %d of class 'VehicleData'", i, field);
                pp->position[i] = value.doubleValue(); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'VehicleData'", field);
    }
}

const char *VehicleDataDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr VehicleDataDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void VehicleDataDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    VehicleData *pp = omnetpp::fromAnyPtr<VehicleData>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'VehicleData'", field);
    }
}

Register_Class(RSU_Data)

RSU_Data::RSU_Data(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

RSU_Data::RSU_Data(const RSU_Data& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

RSU_Data::~RSU_Data()
{
    delete [] this->vehicles;
}

RSU_Data& RSU_Data::operator=(const RSU_Data& other)
{
    if (this == &other) return *this;
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
    for (size_t i = 0; i < vehicles_arraysize; i++) {
        this->vehicles[i] = other.vehicles[i];
    }
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
    if (vehicles_arraysize == 0) {
        this->vehicles = nullptr;
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

size_t RSU_Data::getVehiclesArraySize() const
{
    return vehicles_arraysize;
}

const VehicleData& RSU_Data::getVehicles(size_t k) const
{
    if (k >= vehicles_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicles_arraysize, (unsigned long)k);
    return this->vehicles[k];
}

void RSU_Data::setVehiclesArraySize(size_t newSize)
{
    VehicleData *vehicles2 = (newSize==0) ? nullptr : new VehicleData[newSize];
    size_t minSize = vehicles_arraysize < newSize ? vehicles_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        vehicles2[i] = this->vehicles[i];
    delete [] this->vehicles;
    this->vehicles = vehicles2;
    vehicles_arraysize = newSize;
}

void RSU_Data::setVehicles(size_t k, const VehicleData& vehicles)
{
    if (k >= vehicles_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicles_arraysize, (unsigned long)k);
    this->vehicles[k] = vehicles;
}

void RSU_Data::insertVehicles(size_t k, const VehicleData& vehicles)
{
    if (k > vehicles_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicles_arraysize, (unsigned long)k);
    size_t newSize = vehicles_arraysize + 1;
    VehicleData *vehicles2 = new VehicleData[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        vehicles2[i] = this->vehicles[i];
    vehicles2[k] = vehicles;
    for (i = k + 1; i < newSize; i++)
        vehicles2[i] = this->vehicles[i-1];
    delete [] this->vehicles;
    this->vehicles = vehicles2;
    vehicles_arraysize = newSize;
}

void RSU_Data::appendVehicles(const VehicleData& vehicles)
{
    insertVehicles(vehicles_arraysize, vehicles);
}

void RSU_Data::eraseVehicles(size_t k)
{
    if (k >= vehicles_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)vehicles_arraysize, (unsigned long)k);
    size_t newSize = vehicles_arraysize - 1;
    VehicleData *vehicles2 = (newSize == 0) ? nullptr : new VehicleData[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        vehicles2[i] = this->vehicles[i];
    for (i = k; i < newSize; i++)
        vehicles2[i] = this->vehicles[i+1];
    delete [] this->vehicles;
    this->vehicles = vehicles2;
    vehicles_arraysize = newSize;
}

class RSU_DataDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_rsuId,
        FIELD_lastStepOccupancy,
        FIELD_lastStepMeanSpeed,
        FIELD_lastStepHaltingVehiclesNumber,
        FIELD_vehicles,
    };
  public:
    RSU_DataDescriptor();
    virtual ~RSU_DataDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(RSU_DataDescriptor)

RSU_DataDescriptor::RSU_DataDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(RSU_Data)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

RSU_DataDescriptor::~RSU_DataDescriptor()
{
    delete[] propertyNames;
}

bool RSU_DataDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<RSU_Data *>(obj)!=nullptr;
}

const char **RSU_DataDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *RSU_DataDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int RSU_DataDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 5+base->getFieldCount() : 5;
}

unsigned int RSU_DataDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_rsuId
        FD_ISEDITABLE,    // FIELD_lastStepOccupancy
        FD_ISEDITABLE,    // FIELD_lastStepMeanSpeed
        FD_ISEDITABLE,    // FIELD_lastStepHaltingVehiclesNumber
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISRESIZABLE,    // FIELD_vehicles
    };
    return (field >= 0 && field < 5) ? fieldTypeFlags[field] : 0;
}

const char *RSU_DataDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "rsuId",
        "lastStepOccupancy",
        "lastStepMeanSpeed",
        "lastStepHaltingVehiclesNumber",
        "vehicles",
    };
    return (field >= 0 && field < 5) ? fieldNames[field] : nullptr;
}

int RSU_DataDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "rsuId") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "lastStepOccupancy") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "lastStepMeanSpeed") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "lastStepHaltingVehiclesNumber") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "vehicles") == 0) return baseIndex + 4;
    return base ? base->findField(fieldName) : -1;
}

const char *RSU_DataDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_rsuId
        "int",    // FIELD_lastStepOccupancy
        "double",    // FIELD_lastStepMeanSpeed
        "int",    // FIELD_lastStepHaltingVehiclesNumber
        "VehicleData",    // FIELD_vehicles
    };
    return (field >= 0 && field < 5) ? fieldTypeStrings[field] : nullptr;
}

const char **RSU_DataDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *RSU_DataDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int RSU_DataDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_vehicles: return pp->getVehiclesArraySize();
        default: return 0;
    }
}

void RSU_DataDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_vehicles: pp->setVehiclesArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'RSU_Data'", field);
    }
}

const char *RSU_DataDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string RSU_DataDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_rsuId: return oppstring2string(pp->getRsuId());
        case FIELD_lastStepOccupancy: return long2string(pp->getLastStepOccupancy());
        case FIELD_lastStepMeanSpeed: return double2string(pp->getLastStepMeanSpeed());
        case FIELD_lastStepHaltingVehiclesNumber: return long2string(pp->getLastStepHaltingVehiclesNumber());
        case FIELD_vehicles: return "";
        default: return "";
    }
}

void RSU_DataDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_rsuId: pp->setRsuId((value)); break;
        case FIELD_lastStepOccupancy: pp->setLastStepOccupancy(string2long(value)); break;
        case FIELD_lastStepMeanSpeed: pp->setLastStepMeanSpeed(string2double(value)); break;
        case FIELD_lastStepHaltingVehiclesNumber: pp->setLastStepHaltingVehiclesNumber(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RSU_Data'", field);
    }
}

omnetpp::cValue RSU_DataDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_rsuId: return pp->getRsuId();
        case FIELD_lastStepOccupancy: return pp->getLastStepOccupancy();
        case FIELD_lastStepMeanSpeed: return pp->getLastStepMeanSpeed();
        case FIELD_lastStepHaltingVehiclesNumber: return pp->getLastStepHaltingVehiclesNumber();
        case FIELD_vehicles: return omnetpp::toAnyPtr(&pp->getVehicles(i)); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'RSU_Data' as cValue -- field index out of range?", field);
    }
}

void RSU_DataDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_rsuId: pp->setRsuId(value.stringValue()); break;
        case FIELD_lastStepOccupancy: pp->setLastStepOccupancy(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_lastStepMeanSpeed: pp->setLastStepMeanSpeed(value.doubleValue()); break;
        case FIELD_lastStepHaltingVehiclesNumber: pp->setLastStepHaltingVehiclesNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RSU_Data'", field);
    }
}

const char *RSU_DataDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_vehicles: return omnetpp::opp_typename(typeid(VehicleData));
        default: return nullptr;
    };
}

omnetpp::any_ptr RSU_DataDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        case FIELD_vehicles: return omnetpp::toAnyPtr(&pp->getVehicles(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void RSU_DataDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    RSU_Data *pp = omnetpp::fromAnyPtr<RSU_Data>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'RSU_Data'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp


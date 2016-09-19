#include <string.h>

#include <string>
#include <vector>
#include <map>

#include <otf2/otf2.h>


class Otf2Definitions
{
public:
    Otf2Definitions( OTF2_Reader* reader );
    ~Otf2Definitions();


    struct ClockProperties
    {
        uint64_t timerResolution;
        uint64_t globalOffset;
        uint64_t traceLength;
    };
    typedef std::vector< ClockProperties* > ClockPropertiesT;
    typedef ClockPropertiesT::iterator      ClockPropertiesIT;
    typedef ClockPropertiesT::value_type    ClockPropertiesV;
    ClockPropertiesT m_ClockPropertiess;

    struct Paradigm
    {
        OTF2_Paradigm      paradigm;
        OTF2_StringRef     name;
        OTF2_ParadigmClass paradigmClass;
    };
    typedef std::vector< Paradigm* > ParadigmT;
    typedef ParadigmT::iterator      ParadigmIT;
    typedef ParadigmT::value_type    ParadigmV;
    ParadigmT m_Paradigms;

    struct ParadigmProperty
    {
        OTF2_Paradigm         paradigm;
        OTF2_ParadigmProperty property;
        OTF2_Type             type;
        OTF2_AttributeValue   value;
    };
    typedef std::vector< ParadigmProperty* > ParadigmPropertyT;
    typedef ParadigmPropertyT::iterator      ParadigmPropertyIT;
    typedef ParadigmPropertyT::value_type    ParadigmPropertyV;
    ParadigmPropertyT m_ParadigmPropertys;


    struct MetricClassRecorder
    {
        OTF2_LocationRef recorder;
    };
    typedef std::vector< MetricClassRecorder* > MetricClassRecorderT;
    typedef MetricClassRecorderT::iterator      MetricClassRecorderIT;
    typedef MetricClassRecorderT::value_type    MetricClassRecorderV;

    struct SystemTreeNodeProperty
    {
        OTF2_StringRef      name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< SystemTreeNodeProperty* > SystemTreeNodePropertyT;
    typedef SystemTreeNodePropertyT::iterator      SystemTreeNodePropertyIT;
    typedef SystemTreeNodePropertyT::value_type    SystemTreeNodePropertyV;

    struct SystemTreeNodeDomain
    {
        OTF2_SystemTreeDomain systemTreeDomain;
    };
    typedef std::vector< SystemTreeNodeDomain* > SystemTreeNodeDomainT;
    typedef SystemTreeNodeDomainT::iterator      SystemTreeNodeDomainIT;
    typedef SystemTreeNodeDomainT::value_type    SystemTreeNodeDomainV;

    struct LocationGroupProperty
    {
        OTF2_StringRef      name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< LocationGroupProperty* > LocationGroupPropertyT;
    typedef LocationGroupPropertyT::iterator      LocationGroupPropertyIT;
    typedef LocationGroupPropertyT::value_type    LocationGroupPropertyV;

    struct LocationProperty
    {
        OTF2_StringRef      name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< LocationProperty* > LocationPropertyT;
    typedef LocationPropertyT::iterator      LocationPropertyIT;
    typedef LocationPropertyT::value_type    LocationPropertyV;

    struct CartCoordinate
    {
        uint32_t                rank;
        std::vector< uint32_t > coordinates;
    };
    typedef std::vector< CartCoordinate* > CartCoordinateT;
    typedef CartCoordinateT::iterator      CartCoordinateIT;
    typedef CartCoordinateT::value_type    CartCoordinateV;

    struct CallingContextProperty
    {
        OTF2_StringRef      name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< CallingContextProperty* > CallingContextPropertyT;
    typedef CallingContextPropertyT::iterator      CallingContextPropertyIT;
    typedef CallingContextPropertyT::value_type    CallingContextPropertyV;


    typedef std::map< OTF2_StringRef, std::string > StringT;
    typedef StringT::iterator                       StringIT;
    typedef StringT::value_type                     StringV;
    StringT m_Strings;

    enum MetricType
    {
        METRIC_CLASS,
        METRIC_INSTANCE,
    };
    struct Metric
    {
        MetricType metricType;
    };
    typedef std::map< OTF2_MetricRef, Metric* > MetricT;
    typedef MetricT::iterator                   MetricIT;
    typedef MetricT::value_type                 MetricV;
    MetricT m_Metrics;



    struct Attribute
    {
        OTF2_StringRef name;
        OTF2_StringRef description;
        OTF2_Type      type;
    };
    typedef std::map< OTF2_AttributeRef, Attribute* > AttributeT;
    typedef AttributeT::iterator                      AttributeIT;
    typedef AttributeT::value_type                    AttributeV;
    AttributeT m_Attributes;

    struct SystemTreeNode
    {
        OTF2_StringRef          name;
        OTF2_StringRef          className;
        OTF2_SystemTreeNodeRef  parent;
        SystemTreeNodePropertyT systemTreeNodePropertys;
        SystemTreeNodeDomainT   systemTreeNodeDomains;
    };
    typedef std::map< OTF2_SystemTreeNodeRef, SystemTreeNode* > SystemTreeNodeT;
    typedef SystemTreeNodeT::iterator                           SystemTreeNodeIT;
    typedef SystemTreeNodeT::value_type                         SystemTreeNodeV;
    SystemTreeNodeT m_SystemTreeNodes;

    struct LocationGroup
    {
        OTF2_StringRef         name;
        OTF2_LocationGroupType locationGroupType;
        OTF2_SystemTreeNodeRef systemTreeParent;
        LocationGroupPropertyT locationGroupPropertys;
    };
    typedef std::map< OTF2_LocationGroupRef, LocationGroup* > LocationGroupT;
    typedef LocationGroupT::iterator                          LocationGroupIT;
    typedef LocationGroupT::value_type                        LocationGroupV;
    LocationGroupT m_LocationGroups;

    struct Location
    {
        OTF2_StringRef        name;
        OTF2_LocationType     locationType;
        uint64_t              numberOfEvents;
        OTF2_LocationGroupRef locationGroup;
        LocationPropertyT     locationPropertys;
    };
    typedef std::map< OTF2_LocationRef, Location* > LocationT;
    typedef LocationT::iterator                     LocationIT;
    typedef LocationT::value_type                   LocationV;
    LocationT m_Locations;

    struct Region
    {
        OTF2_StringRef  name;
        OTF2_StringRef  canonicalName;
        OTF2_StringRef  description;
        OTF2_RegionRole regionRole;
        OTF2_Paradigm   paradigm;
        OTF2_RegionFlag regionFlags;
        OTF2_StringRef  sourceFile;
        uint32_t        beginLineNumber;
        uint32_t        endLineNumber;
    };
    typedef std::map< OTF2_RegionRef, Region* > RegionT;
    typedef RegionT::iterator                   RegionIT;
    typedef RegionT::value_type                 RegionV;
    RegionT m_Regions;

    struct Callsite
    {
        OTF2_StringRef sourceFile;
        uint32_t       lineNumber;
        OTF2_RegionRef enteredRegion;
        OTF2_RegionRef leftRegion;
    };
    typedef std::map< OTF2_CallsiteRef, Callsite* > CallsiteT;
    typedef CallsiteT::iterator                     CallsiteIT;
    typedef CallsiteT::value_type                   CallsiteV;
    CallsiteT m_Callsites;

    struct Callpath
    {
        OTF2_CallpathRef parent;
        OTF2_RegionRef   region;
    };
    typedef std::map< OTF2_CallpathRef, Callpath* > CallpathT;
    typedef CallpathT::iterator                     CallpathIT;
    typedef CallpathT::value_type                   CallpathV;
    CallpathT m_Callpaths;

    struct Group
    {
        OTF2_StringRef          name;
        OTF2_GroupType          groupType;
        OTF2_Paradigm           paradigm;
        OTF2_GroupFlag          groupFlags;
        std::vector< uint64_t > members;
    };
    typedef std::map< OTF2_GroupRef, Group* > GroupT;
    typedef GroupT::iterator                  GroupIT;
    typedef GroupT::value_type                GroupV;
    GroupT m_Groups;

    struct MetricMember
    {
        OTF2_StringRef  name;
        OTF2_StringRef  description;
        OTF2_MetricType metricType;
        OTF2_MetricMode metricMode;
        OTF2_Type       valueType;
        OTF2_Base       base;
        int64_t         exponent;
        OTF2_StringRef  unit;
    };
    typedef std::map< OTF2_MetricMemberRef, MetricMember* > MetricMemberT;
    typedef MetricMemberT::iterator                         MetricMemberIT;
    typedef MetricMemberT::value_type                       MetricMemberV;
    MetricMemberT m_MetricMembers;

    struct MetricClass
        : Metric
    {
        std::vector< OTF2_MetricMemberRef > metricMembers;
        OTF2_MetricOccurrence               metricOccurrence;
        OTF2_RecorderKind                   recorderKind;
        MetricClassRecorderT                metricClassRecorders;
    };

    struct MetricInstance
        : Metric
    {
        OTF2_MetricRef   metricClass;
        OTF2_LocationRef recorder;
        OTF2_MetricScope metricScope;
        uint64_t         scope;
    };

    struct Comm
    {
        OTF2_StringRef name;
        OTF2_GroupRef  group;
        OTF2_CommRef   parent;
    };
    typedef std::map< OTF2_CommRef, Comm* > CommT;
    typedef CommT::iterator                 CommIT;
    typedef CommT::value_type               CommV;
    CommT m_Comms;

    struct Parameter
    {
        OTF2_StringRef     name;
        OTF2_ParameterType parameterType;
    };
    typedef std::map< OTF2_ParameterRef, Parameter* > ParameterT;
    typedef ParameterT::iterator                      ParameterIT;
    typedef ParameterT::value_type                    ParameterV;
    ParameterT m_Parameters;

    struct RmaWin
    {
        OTF2_StringRef name;
        OTF2_CommRef   comm;
    };
    typedef std::map< OTF2_RmaWinRef, RmaWin* > RmaWinT;
    typedef RmaWinT::iterator                   RmaWinIT;
    typedef RmaWinT::value_type                 RmaWinV;
    RmaWinT m_RmaWins;

    struct CartDimension
    {
        OTF2_StringRef       name;
        uint32_t             size;
        OTF2_CartPeriodicity cartPeriodicity;
    };
    typedef std::map< OTF2_CartDimensionRef, CartDimension* > CartDimensionT;
    typedef CartDimensionT::iterator                          CartDimensionIT;
    typedef CartDimensionT::value_type                        CartDimensionV;
    CartDimensionT m_CartDimensions;

    struct CartTopology
    {
        OTF2_StringRef                       name;
        OTF2_CommRef                         communicator;
        std::vector< OTF2_CartDimensionRef > cartDimensions;
        CartCoordinateT                      cartCoordinates;
    };
    typedef std::map< OTF2_CartTopologyRef, CartTopology* > CartTopologyT;
    typedef CartTopologyT::iterator                         CartTopologyIT;
    typedef CartTopologyT::value_type                       CartTopologyV;
    CartTopologyT m_CartTopologys;

    struct SourceCodeLocation
    {
        OTF2_StringRef file;
        uint32_t       lineNumber;
    };
    typedef std::map< OTF2_SourceCodeLocationRef, SourceCodeLocation* > SourceCodeLocationT;
    typedef SourceCodeLocationT::iterator                               SourceCodeLocationIT;
    typedef SourceCodeLocationT::value_type                             SourceCodeLocationV;
    SourceCodeLocationT m_SourceCodeLocations;

    struct CallingContext
    {
        OTF2_RegionRef             region;
        OTF2_SourceCodeLocationRef sourceCodeLocation;
        OTF2_CallingContextRef     parent;
        CallingContextPropertyT    callingContextPropertys;
    };
    typedef std::map< OTF2_CallingContextRef, CallingContext* > CallingContextT;
    typedef CallingContextT::iterator                           CallingContextIT;
    typedef CallingContextT::value_type                         CallingContextV;
    CallingContextT m_CallingContexts;

    struct InterruptGenerator
    {
        OTF2_StringRef              name;
        OTF2_InterruptGeneratorMode interruptGeneratorMode;
        OTF2_Base                   base;
        int64_t                     exponent;
        uint64_t                    period;
    };
    typedef std::map< OTF2_InterruptGeneratorRef, InterruptGenerator* > InterruptGeneratorT;
    typedef InterruptGeneratorT::iterator                               InterruptGeneratorIT;
    typedef InterruptGeneratorT::value_type                             InterruptGeneratorV;
    InterruptGeneratorT m_InterruptGenerators;

    OTF2_CallbackCode
    HandleDefClockProperties( uint64_t timerResolution,
                              uint64_t globalOffset,
                              uint64_t traceLength );

    OTF2_CallbackCode
    HandleDefParadigm( OTF2_Paradigm      paradigm,
                       OTF2_StringRef     name,
                       OTF2_ParadigmClass paradigmClass );

    OTF2_CallbackCode
    HandleDefParadigmProperty( OTF2_Paradigm         paradigm,
                               OTF2_ParadigmProperty property,
                               OTF2_Type             type,
                               OTF2_AttributeValue   value );

    OTF2_CallbackCode
    HandleDefString( OTF2_StringRef self,
                     const char*    string );

    OTF2_CallbackCode
    HandleDefAttribute( OTF2_AttributeRef self,
                        OTF2_StringRef    name,
                        OTF2_StringRef    description,
                        OTF2_Type         type );

    OTF2_CallbackCode
    HandleDefSystemTreeNode( OTF2_SystemTreeNodeRef self,
                             OTF2_StringRef         name,
                             OTF2_StringRef         className,
                             OTF2_SystemTreeNodeRef parent );

    OTF2_CallbackCode
    HandleDefLocationGroup( OTF2_LocationGroupRef  self,
                            OTF2_StringRef         name,
                            OTF2_LocationGroupType locationGroupType,
                            OTF2_SystemTreeNodeRef systemTreeParent );

    OTF2_CallbackCode
    HandleDefLocation( OTF2_LocationRef      self,
                       OTF2_StringRef        name,
                       OTF2_LocationType     locationType,
                       uint64_t              numberOfEvents,
                       OTF2_LocationGroupRef locationGroup );

    OTF2_CallbackCode
    HandleDefRegion( OTF2_RegionRef  self,
                     OTF2_StringRef  name,
                     OTF2_StringRef  canonicalName,
                     OTF2_StringRef  description,
                     OTF2_RegionRole regionRole,
                     OTF2_Paradigm   paradigm,
                     OTF2_RegionFlag regionFlags,
                     OTF2_StringRef  sourceFile,
                     uint32_t        beginLineNumber,
                     uint32_t        endLineNumber );

    OTF2_CallbackCode
    HandleDefCallsite( OTF2_CallsiteRef self,
                       OTF2_StringRef   sourceFile,
                       uint32_t         lineNumber,
                       OTF2_RegionRef   enteredRegion,
                       OTF2_RegionRef   leftRegion );

    OTF2_CallbackCode
    HandleDefCallpath( OTF2_CallpathRef self,
                       OTF2_CallpathRef parent,
                       OTF2_RegionRef   region );

    OTF2_CallbackCode
    HandleDefGroup( OTF2_GroupRef   self,
                    OTF2_StringRef  name,
                    OTF2_GroupType  groupType,
                    OTF2_Paradigm   paradigm,
                    OTF2_GroupFlag  groupFlags,
                    uint32_t        numberOfMembers,
                    const uint64_t* members );

    OTF2_CallbackCode
    HandleDefMetricMember( OTF2_MetricMemberRef self,
                           OTF2_StringRef       name,
                           OTF2_StringRef       description,
                           OTF2_MetricType      metricType,
                           OTF2_MetricMode      metricMode,
                           OTF2_Type            valueType,
                           OTF2_Base            base,
                           int64_t              exponent,
                           OTF2_StringRef       unit );

    OTF2_CallbackCode
    HandleDefMetricClass( OTF2_MetricRef              self,
                          uint8_t                     numberOfMetrics,
                          const OTF2_MetricMemberRef* metricMembers,
                          OTF2_MetricOccurrence       metricOccurrence,
                          OTF2_RecorderKind           recorderKind );

    OTF2_CallbackCode
    HandleDefMetricInstance( OTF2_MetricRef   self,
                             OTF2_MetricRef   metricClass,
                             OTF2_LocationRef recorder,
                             OTF2_MetricScope metricScope,
                             uint64_t         scope );

    OTF2_CallbackCode
    HandleDefComm( OTF2_CommRef   self,
                   OTF2_StringRef name,
                   OTF2_GroupRef  group,
                   OTF2_CommRef   parent );

    OTF2_CallbackCode
    HandleDefParameter( OTF2_ParameterRef  self,
                        OTF2_StringRef     name,
                        OTF2_ParameterType parameterType );

    OTF2_CallbackCode
    HandleDefRmaWin( OTF2_RmaWinRef self,
                     OTF2_StringRef name,
                     OTF2_CommRef   comm );

    OTF2_CallbackCode
    HandleDefMetricClassRecorder( OTF2_MetricRef   metricClass,
                                  OTF2_LocationRef recorder );

    OTF2_CallbackCode
    HandleDefSystemTreeNodeProperty( OTF2_SystemTreeNodeRef systemTreeNode,
                                     OTF2_StringRef         name,
                                     OTF2_Type              type,
                                     OTF2_AttributeValue    value );

    OTF2_CallbackCode
    HandleDefSystemTreeNodeDomain( OTF2_SystemTreeNodeRef systemTreeNode,
                                   OTF2_SystemTreeDomain  systemTreeDomain );

    OTF2_CallbackCode
    HandleDefLocationGroupProperty( OTF2_LocationGroupRef locationGroup,
                                    OTF2_StringRef        name,
                                    OTF2_Type             type,
                                    OTF2_AttributeValue   value );

    OTF2_CallbackCode
    HandleDefLocationProperty( OTF2_LocationRef    location,
                               OTF2_StringRef      name,
                               OTF2_Type           type,
                               OTF2_AttributeValue value );

    OTF2_CallbackCode
    HandleDefCartDimension( OTF2_CartDimensionRef self,
                            OTF2_StringRef        name,
                            uint32_t              size,
                            OTF2_CartPeriodicity  cartPeriodicity );

    OTF2_CallbackCode
    HandleDefCartTopology( OTF2_CartTopologyRef         self,
                           OTF2_StringRef               name,
                           OTF2_CommRef                 communicator,
                           uint8_t                      numberOfDimensions,
                           const OTF2_CartDimensionRef* cartDimensions );

    OTF2_CallbackCode
    HandleDefCartCoordinate( OTF2_CartTopologyRef cartTopology,
                             uint32_t             rank,
                             uint8_t              numberOfDimensions,
                             const uint32_t*      coordinates );

    OTF2_CallbackCode
    HandleDefSourceCodeLocation( OTF2_SourceCodeLocationRef self,
                                 OTF2_StringRef             file,
                                 uint32_t                   lineNumber );

    OTF2_CallbackCode
    HandleDefCallingContext( OTF2_CallingContextRef     self,
                             OTF2_RegionRef             region,
                             OTF2_SourceCodeLocationRef sourceCodeLocation,
                             OTF2_CallingContextRef     parent );

    OTF2_CallbackCode
    HandleDefCallingContextProperty( OTF2_CallingContextRef callingContext,
                                     OTF2_StringRef         name,
                                     OTF2_Type              type,
                                     OTF2_AttributeValue    value );

    OTF2_CallbackCode
    HandleDefInterruptGenerator( OTF2_InterruptGeneratorRef  self,
                                 OTF2_StringRef              name,
                                 OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                 OTF2_Base                   base,
                                 int64_t                     exponent,
                                 uint64_t                    period );

private:
    OTF2_Reader* m_Reader;
};


OTF2_CallbackCode
Otf2Definitions::HandleDefClockProperties( uint64_t timerResolution,
                                           uint64_t globalOffset,
                                           uint64_t traceLength )
{
    ClockProperties& new_clock_properties = *( new ClockProperties );

    new_clock_properties.timerResolution = timerResolution;
    new_clock_properties.globalOffset    = globalOffset;
    new_clock_properties.traceLength     = traceLength;

    m_ClockPropertiess.push_back( &new_clock_properties );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefParadigm( OTF2_Paradigm      paradigm,
                                    OTF2_StringRef     name,
                                    OTF2_ParadigmClass paradigmClass )
{
    Paradigm& new_paradigm = *( new Paradigm );

    new_paradigm.paradigm      = paradigm;
    new_paradigm.name          = name;
    new_paradigm.paradigmClass = paradigmClass;

    m_Paradigms.push_back( &new_paradigm );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefParadigmProperty( OTF2_Paradigm         paradigm,
                                            OTF2_ParadigmProperty property,
                                            OTF2_Type             type,
                                            OTF2_AttributeValue   value )
{
    ParadigmProperty& new_paradigm_property = *( new ParadigmProperty );

    new_paradigm_property.paradigm = paradigm;
    new_paradigm_property.property = property;
    new_paradigm_property.type     = type;
    new_paradigm_property.value    = value;

    m_ParadigmPropertys.push_back( &new_paradigm_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricClassRecorder( OTF2_MetricRef   metricClass,
                                               OTF2_LocationRef recorder )
{
    MetricClassIT        parent_it                 = m_MetricClasss.find( metricClass );
    MetricClassRecorder& new_metric_class_recorder = *( new MetricClassRecorder );

    new_metric_class_recorder.recorder = recorder;

    parent_it->second->metricClassRecorders.push_back( &new_metric_class_recorder );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSystemTreeNodeProperty( OTF2_SystemTreeNodeRef systemTreeNode,
                                                  OTF2_StringRef         name,
                                                  OTF2_Type              type,
                                                  OTF2_AttributeValue    value )
{
    SystemTreeNodeIT        parent_it                     = m_SystemTreeNodes.find( systemTreeNode );
    SystemTreeNodeProperty& new_system_tree_node_property = *( new SystemTreeNodeProperty );

    new_system_tree_node_property.name  = name;
    new_system_tree_node_property.type  = type;
    new_system_tree_node_property.value = value;

    parent_it->second->systemTreeNodePropertys.push_back( &new_system_tree_node_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSystemTreeNodeDomain( OTF2_SystemTreeNodeRef systemTreeNode,
                                                OTF2_SystemTreeDomain  systemTreeDomain )
{
    SystemTreeNodeIT      parent_it                   = m_SystemTreeNodes.find( systemTreeNode );
    SystemTreeNodeDomain& new_system_tree_node_domain = *( new SystemTreeNodeDomain );

    new_system_tree_node_domain.systemTreeDomain = systemTreeDomain;

    parent_it->second->systemTreeNodeDomains.push_back( &new_system_tree_node_domain );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocationGroupProperty( OTF2_LocationGroupRef locationGroup,
                                                 OTF2_StringRef        name,
                                                 OTF2_Type             type,
                                                 OTF2_AttributeValue   value )
{
    LocationGroupIT        parent_it                   = m_LocationGroups.find( locationGroup );
    LocationGroupProperty& new_location_group_property = *( new LocationGroupProperty );

    new_location_group_property.name  = name;
    new_location_group_property.type  = type;
    new_location_group_property.value = value;

    parent_it->second->locationGroupPropertys.push_back( &new_location_group_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocationProperty( OTF2_LocationRef    location,
                                            OTF2_StringRef      name,
                                            OTF2_Type           type,
                                            OTF2_AttributeValue value )
{
    LocationIT        parent_it             = m_Locations.find( location );
    LocationProperty& new_location_property = *( new LocationProperty );

    new_location_property.name  = name;
    new_location_property.type  = type;
    new_location_property.value = value;

    parent_it->second->locationPropertys.push_back( &new_location_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCartCoordinate( OTF2_CartTopologyRef cartTopology,
                                          uint32_t             rank,
                                          uint8_t              numberOfDimensions,
                                          const uint32_t*      coordinates )
{
    CartTopologyIT  parent_it           = m_CartTopologys.find( cartTopology );
    CartCoordinate& new_cart_coordinate = *( new CartCoordinate );

    new_cart_coordinate.rank = rank;
    std::vector< uint32_t > coordinates_vector( coordinates, coordinates + numberOfDimensions );
    new_cart_coordinate.coordinates = coordinates_vector;

    parent_it->second->cartCoordinates.push_back( &new_cart_coordinate );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallingContextProperty( OTF2_CallingContextRef callingContext,
                                                  OTF2_StringRef         name,
                                                  OTF2_Type              type,
                                                  OTF2_AttributeValue    value )
{
    CallingContextIT        parent_it                    = m_CallingContexts.find( callingContext );
    CallingContextProperty& new_calling_context_property = *( new CallingContextProperty );

    new_calling_context_property.name  = name;
    new_calling_context_property.type  = type;
    new_calling_context_property.value = value;

    parent_it->second->callingContextPropertys.push_back( &new_calling_context_property );

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
Otf2Definitions::HandleDefString( OTF2_StringRef stringID,
                                  const char*    string )
{
    m_Strings[ stringID ] = std::string( string );

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
Otf2Definitions::HandleDefAttribute( OTF2_AttributeRef self,
                                     OTF2_StringRef    name,
                                     OTF2_StringRef    description,
                                     OTF2_Type         type )
{
    Attribute& new_attribute = *( new Attribute );

    new_attribute.name        = name;
    new_attribute.description = description;
    new_attribute.type        = type;

    m_Attributes[ self ] = &new_attribute;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSystemTreeNode( OTF2_SystemTreeNodeRef self,
                                          OTF2_StringRef         name,
                                          OTF2_StringRef         className,
                                          OTF2_SystemTreeNodeRef parent )
{
    SystemTreeNode& new_system_tree_node = *( new SystemTreeNode );

    new_system_tree_node.name      = name;
    new_system_tree_node.className = className;
    new_system_tree_node.parent    = parent;

    m_SystemTreeNodes[ self ] = &new_system_tree_node;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocationGroup( OTF2_LocationGroupRef  self,
                                         OTF2_StringRef         name,
                                         OTF2_LocationGroupType locationGroupType,
                                         OTF2_SystemTreeNodeRef systemTreeParent )
{
    LocationGroup& new_location_group = *( new LocationGroup );

    new_location_group.name              = name;
    new_location_group.locationGroupType = locationGroupType;
    new_location_group.systemTreeParent  = systemTreeParent;

    m_LocationGroups[ self ] = &new_location_group;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocation( OTF2_LocationRef      self,
                                    OTF2_StringRef        name,
                                    OTF2_LocationType     locationType,
                                    uint64_t              numberOfEvents,
                                    OTF2_LocationGroupRef locationGroup )
{
    Location& new_location = *( new Location );

    new_location.name           = name;
    new_location.locationType   = locationType;
    new_location.numberOfEvents = numberOfEvents;
    new_location.locationGroup  = locationGroup;

    m_Locations[ self ] = &new_location;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefRegion( OTF2_RegionRef  self,
                                  OTF2_StringRef  name,
                                  OTF2_StringRef  canonicalName,
                                  OTF2_StringRef  description,
                                  OTF2_RegionRole regionRole,
                                  OTF2_Paradigm   paradigm,
                                  OTF2_RegionFlag regionFlags,
                                  OTF2_StringRef  sourceFile,
                                  uint32_t        beginLineNumber,
                                  uint32_t        endLineNumber )
{
    Region& new_region = *( new Region );

    new_region.name            = name;
    new_region.canonicalName   = canonicalName;
    new_region.description     = description;
    new_region.regionRole      = regionRole;
    new_region.paradigm        = paradigm;
    new_region.regionFlags     = regionFlags;
    new_region.sourceFile      = sourceFile;
    new_region.beginLineNumber = beginLineNumber;
    new_region.endLineNumber   = endLineNumber;

    m_Regions[ self ] = &new_region;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallsite( OTF2_CallsiteRef self,
                                    OTF2_StringRef   sourceFile,
                                    uint32_t         lineNumber,
                                    OTF2_RegionRef   enteredRegion,
                                    OTF2_RegionRef   leftRegion )
{
    Callsite& new_callsite = *( new Callsite );

    new_callsite.sourceFile    = sourceFile;
    new_callsite.lineNumber    = lineNumber;
    new_callsite.enteredRegion = enteredRegion;
    new_callsite.leftRegion    = leftRegion;

    m_Callsites[ self ] = &new_callsite;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallpath( OTF2_CallpathRef self,
                                    OTF2_CallpathRef parent,
                                    OTF2_RegionRef   region )
{
    Callpath& new_callpath = *( new Callpath );

    new_callpath.parent = parent;
    new_callpath.region = region;

    m_Callpaths[ self ] = &new_callpath;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefGroup( OTF2_GroupRef   self,
                                 OTF2_StringRef  name,
                                 OTF2_GroupType  groupType,
                                 OTF2_Paradigm   paradigm,
                                 OTF2_GroupFlag  groupFlags,
                                 uint32_t        numberOfMembers,
                                 const uint64_t* members )
{
    Group& new_group = *( new Group );

    new_group.name       = name;
    new_group.groupType  = groupType;
    new_group.paradigm   = paradigm;
    new_group.groupFlags = groupFlags;
    std::vector< uint64_t > members_vector( members, members + numberOfMembers );
    new_group.members = members_vector;

    m_Groups[ self ] = &new_group;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricMember( OTF2_MetricMemberRef self,
                                        OTF2_StringRef       name,
                                        OTF2_StringRef       description,
                                        OTF2_MetricType      metricType,
                                        OTF2_MetricMode      metricMode,
                                        OTF2_Type            valueType,
                                        OTF2_Base            base,
                                        int64_t              exponent,
                                        OTF2_StringRef       unit )
{
    MetricMember& new_metric_member = *( new MetricMember );

    new_metric_member.name        = name;
    new_metric_member.description = description;
    new_metric_member.metricType  = metricType;
    new_metric_member.metricMode  = metricMode;
    new_metric_member.valueType   = valueType;
    new_metric_member.base        = base;
    new_metric_member.exponent    = exponent;
    new_metric_member.unit        = unit;

    m_MetricMembers[ self ] = &new_metric_member;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricClass( OTF2_MetricRef              self,
                                       uint8_t                     numberOfMetrics,
                                       const OTF2_MetricMemberRef* metricMembers,
                                       OTF2_MetricOccurrence       metricOccurrence,
                                       OTF2_RecorderKind           recorderKind )
{
    MetricClass& new_metric_class = *( new MetricClass );

    new_metric_class.metricType = METRIC_CLASS;

    std::vector< OTF2_MetricMemberRef > metricMembers_vector( metricMembers, metricMembers + numberOfMetrics );
    new_metric_class.metricMembers    = metricMembers_vector;
    new_metric_class.metricOccurrence = metricOccurrence;
    new_metric_class.recorderKind     = recorderKind;

    m_Metrics[ self ] = &new_metric_class;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricInstance( OTF2_MetricRef   self,
                                          OTF2_MetricRef   metricClass,
                                          OTF2_LocationRef recorder,
                                          OTF2_MetricScope metricScope,
                                          uint64_t         scope )
{
    MetricInstance& new_metric_instance = *( new MetricInstance );

    new_metric_instance.metricType = METRIC_INSTANCE;

    new_metric_instance.metricClass = metricClass;
    new_metric_instance.recorder    = recorder;
    new_metric_instance.metricScope = metricScope;
    new_metric_instance.scope       = scope;

    m_Metrics[ self ] = &new_metric_instance;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefComm( OTF2_CommRef   self,
                                OTF2_StringRef name,
                                OTF2_GroupRef  group,
                                OTF2_CommRef   parent )
{
    Comm& new_comm = *( new Comm );

    new_comm.name   = name;
    new_comm.group  = group;
    new_comm.parent = parent;

    m_Comms[ self ] = &new_comm;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefParameter( OTF2_ParameterRef  self,
                                     OTF2_StringRef     name,
                                     OTF2_ParameterType parameterType )
{
    Parameter& new_parameter = *( new Parameter );

    new_parameter.name          = name;
    new_parameter.parameterType = parameterType;

    m_Parameters[ self ] = &new_parameter;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefRmaWin( OTF2_RmaWinRef self,
                                  OTF2_StringRef name,
                                  OTF2_CommRef   comm )
{
    RmaWin& new_rma_win = *( new RmaWin );

    new_rma_win.name = name;
    new_rma_win.comm = comm;

    m_RmaWins[ self ] = &new_rma_win;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCartDimension( OTF2_CartDimensionRef self,
                                         OTF2_StringRef        name,
                                         uint32_t              size,
                                         OTF2_CartPeriodicity  cartPeriodicity )
{
    CartDimension& new_cart_dimension = *( new CartDimension );

    new_cart_dimension.name            = name;
    new_cart_dimension.size            = size;
    new_cart_dimension.cartPeriodicity = cartPeriodicity;

    m_CartDimensions[ self ] = &new_cart_dimension;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCartTopology( OTF2_CartTopologyRef         self,
                                        OTF2_StringRef               name,
                                        OTF2_CommRef                 communicator,
                                        uint8_t                      numberOfDimensions,
                                        const OTF2_CartDimensionRef* cartDimensions )
{
    CartTopology& new_cart_topology = *( new CartTopology );

    new_cart_topology.name         = name;
    new_cart_topology.communicator = communicator;
    std::vector< OTF2_CartDimensionRef > cartDimensions_vector( cartDimensions, cartDimensions + numberOfDimensions );
    new_cart_topology.cartDimensions = cartDimensions_vector;

    m_CartTopologys[ self ] = &new_cart_topology;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSourceCodeLocation( OTF2_SourceCodeLocationRef self,
                                              OTF2_StringRef             file,
                                              uint32_t                   lineNumber )
{
    SourceCodeLocation& new_source_code_location = *( new SourceCodeLocation );

    new_source_code_location.file       = file;
    new_source_code_location.lineNumber = lineNumber;

    m_SourceCodeLocations[ self ] = &new_source_code_location;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallingContext( OTF2_CallingContextRef     self,
                                          OTF2_RegionRef             region,
                                          OTF2_SourceCodeLocationRef sourceCodeLocation,
                                          OTF2_CallingContextRef     parent )
{
    CallingContext& new_calling_context = *( new CallingContext );

    new_calling_context.region             = region;
    new_calling_context.sourceCodeLocation = sourceCodeLocation;
    new_calling_context.parent             = parent;

    m_CallingContexts[ self ] = &new_calling_context;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefInterruptGenerator( OTF2_InterruptGeneratorRef  self,
                                              OTF2_StringRef              name,
                                              OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                              OTF2_Base                   base,
                                              int64_t                     exponent,
                                              uint64_t                    period )
{
    InterruptGenerator& new_interrupt_generator = *( new InterruptGenerator );

    new_interrupt_generator.name                   = name;
    new_interrupt_generator.interruptGeneratorMode = interruptGeneratorMode;
    new_interrupt_generator.base                   = base;
    new_interrupt_generator.exponent               = exponent;
    new_interrupt_generator.period                 = period;

    m_InterruptGenerators[ self ] = &new_interrupt_generator;

    return OTF2_CALLBACK_SUCCESS;
}


extern "C" {
static OTF2_CallbackCode
clock_properties_forward_cb( void*    userData,
                             uint64_t timerResolution,
                             uint64_t globalOffset,
                             uint64_t traceLength )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefClockProperties( timerResolution,
                                             globalOffset,
                                             traceLength );
}


static OTF2_CallbackCode
paradigm_forward_cb( void*              userData,
                     OTF2_Paradigm      paradigm,
                     OTF2_StringRef     name,
                     OTF2_ParadigmClass paradigmClass )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefParadigm( paradigm,
                                      name,
                                      paradigmClass );
}


static OTF2_CallbackCode
paradigm_property_forward_cb( void*                 userData,
                              OTF2_Paradigm         paradigm,
                              OTF2_ParadigmProperty property,
                              OTF2_Type             type,
                              OTF2_AttributeValue   value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefParadigmProperty( paradigm,
                                              property,
                                              type,
                                              value );
}


static OTF2_CallbackCode
string_forward_cb( void*          userData,
                   OTF2_StringRef self,
                   const char*    string )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefString( self,
                                    string );
}


static OTF2_CallbackCode
attribute_forward_cb( void*             userData,
                      OTF2_AttributeRef self,
                      OTF2_StringRef    name,
                      OTF2_StringRef    description,
                      OTF2_Type         type )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefAttribute( self,
                                       name,
                                       description,
                                       type );
}


static OTF2_CallbackCode
system_tree_node_forward_cb( void*                  userData,
                             OTF2_SystemTreeNodeRef self,
                             OTF2_StringRef         name,
                             OTF2_StringRef         className,
                             OTF2_SystemTreeNodeRef parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSystemTreeNode( self,
                                            name,
                                            className,
                                            parent );
}


static OTF2_CallbackCode
location_group_forward_cb( void*                  userData,
                           OTF2_LocationGroupRef  self,
                           OTF2_StringRef         name,
                           OTF2_LocationGroupType locationGroupType,
                           OTF2_SystemTreeNodeRef systemTreeParent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocationGroup( self,
                                           name,
                                           locationGroupType,
                                           systemTreeParent );
}


static OTF2_CallbackCode
location_forward_cb( void*                 userData,
                     OTF2_LocationRef      self,
                     OTF2_StringRef        name,
                     OTF2_LocationType     locationType,
                     uint64_t              numberOfEvents,
                     OTF2_LocationGroupRef locationGroup )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocation( self,
                                      name,
                                      locationType,
                                      numberOfEvents,
                                      locationGroup );
}


static OTF2_CallbackCode
region_forward_cb( void*           userData,
                   OTF2_RegionRef  self,
                   OTF2_StringRef  name,
                   OTF2_StringRef  canonicalName,
                   OTF2_StringRef  description,
                   OTF2_RegionRole regionRole,
                   OTF2_Paradigm   paradigm,
                   OTF2_RegionFlag regionFlags,
                   OTF2_StringRef  sourceFile,
                   uint32_t        beginLineNumber,
                   uint32_t        endLineNumber )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefRegion( self,
                                    name,
                                    canonicalName,
                                    description,
                                    regionRole,
                                    paradigm,
                                    regionFlags,
                                    sourceFile,
                                    beginLineNumber,
                                    endLineNumber );
}


static OTF2_CallbackCode
callsite_forward_cb( void*            userData,
                     OTF2_CallsiteRef self,
                     OTF2_StringRef   sourceFile,
                     uint32_t         lineNumber,
                     OTF2_RegionRef   enteredRegion,
                     OTF2_RegionRef   leftRegion )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallsite( self,
                                      sourceFile,
                                      lineNumber,
                                      enteredRegion,
                                      leftRegion );
}


static OTF2_CallbackCode
callpath_forward_cb( void*            userData,
                     OTF2_CallpathRef self,
                     OTF2_CallpathRef parent,
                     OTF2_RegionRef   region )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallpath( self,
                                      parent,
                                      region );
}


static OTF2_CallbackCode
group_forward_cb( void*           userData,
                  OTF2_GroupRef   self,
                  OTF2_StringRef  name,
                  OTF2_GroupType  groupType,
                  OTF2_Paradigm   paradigm,
                  OTF2_GroupFlag  groupFlags,
                  uint32_t        numberOfMembers,
                  const uint64_t* members )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefGroup( self,
                                   name,
                                   groupType,
                                   paradigm,
                                   groupFlags,
                                   numberOfMembers,
                                   members );
}


static OTF2_CallbackCode
metric_member_forward_cb( void*                userData,
                          OTF2_MetricMemberRef self,
                          OTF2_StringRef       name,
                          OTF2_StringRef       description,
                          OTF2_MetricType      metricType,
                          OTF2_MetricMode      metricMode,
                          OTF2_Type            valueType,
                          OTF2_Base            base,
                          int64_t              exponent,
                          OTF2_StringRef       unit )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricMember( self,
                                          name,
                                          description,
                                          metricType,
                                          metricMode,
                                          valueType,
                                          base,
                                          exponent,
                                          unit );
}


static OTF2_CallbackCode
metric_class_forward_cb( void*                       userData,
                         OTF2_MetricRef              self,
                         uint8_t                     numberOfMetrics,
                         const OTF2_MetricMemberRef* metricMembers,
                         OTF2_MetricOccurrence       metricOccurrence,
                         OTF2_RecorderKind           recorderKind )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricClass( self,
                                         numberOfMetrics,
                                         metricMembers,
                                         metricOccurrence,
                                         recorderKind );
}


static OTF2_CallbackCode
metric_instance_forward_cb( void*            userData,
                            OTF2_MetricRef   self,
                            OTF2_MetricRef   metricClass,
                            OTF2_LocationRef recorder,
                            OTF2_MetricScope metricScope,
                            uint64_t         scope )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricInstance( self,
                                            metricClass,
                                            recorder,
                                            metricScope,
                                            scope );
}


static OTF2_CallbackCode
comm_forward_cb( void*          userData,
                 OTF2_CommRef   self,
                 OTF2_StringRef name,
                 OTF2_GroupRef  group,
                 OTF2_CommRef   parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefComm( self,
                                  name,
                                  group,
                                  parent );
}


static OTF2_CallbackCode
parameter_forward_cb( void*              userData,
                      OTF2_ParameterRef  self,
                      OTF2_StringRef     name,
                      OTF2_ParameterType parameterType )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefParameter( self,
                                       name,
                                       parameterType );
}


static OTF2_CallbackCode
rma_win_forward_cb( void*          userData,
                    OTF2_RmaWinRef self,
                    OTF2_StringRef name,
                    OTF2_CommRef   comm )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefRmaWin( self,
                                    name,
                                    comm );
}


static OTF2_CallbackCode
metric_class_recorder_forward_cb( void*            userData,
                                  OTF2_MetricRef   metricClass,
                                  OTF2_LocationRef recorder )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricClassRecorder( metricClass,
                                                 recorder );
}


static OTF2_CallbackCode
system_tree_node_property_forward_cb( void*                  userData,
                                      OTF2_SystemTreeNodeRef systemTreeNode,
                                      OTF2_StringRef         name,
                                      OTF2_Type              type,
                                      OTF2_AttributeValue    value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSystemTreeNodeProperty( systemTreeNode,
                                                    name,
                                                    type,
                                                    value );
}


static OTF2_CallbackCode
system_tree_node_domain_forward_cb( void*                  userData,
                                    OTF2_SystemTreeNodeRef systemTreeNode,
                                    OTF2_SystemTreeDomain  systemTreeDomain )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSystemTreeNodeDomain( systemTreeNode,
                                                  systemTreeDomain );
}


static OTF2_CallbackCode
location_group_property_forward_cb( void*                 userData,
                                    OTF2_LocationGroupRef locationGroup,
                                    OTF2_StringRef        name,
                                    OTF2_Type             type,
                                    OTF2_AttributeValue   value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocationGroupProperty( locationGroup,
                                                   name,
                                                   type,
                                                   value );
}


static OTF2_CallbackCode
location_property_forward_cb( void*               userData,
                              OTF2_LocationRef    location,
                              OTF2_StringRef      name,
                              OTF2_Type           type,
                              OTF2_AttributeValue value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocationProperty( location,
                                              name,
                                              type,
                                              value );
}


static OTF2_CallbackCode
cart_dimension_forward_cb( void*                 userData,
                           OTF2_CartDimensionRef self,
                           OTF2_StringRef        name,
                           uint32_t              size,
                           OTF2_CartPeriodicity  cartPeriodicity )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCartDimension( self,
                                           name,
                                           size,
                                           cartPeriodicity );
}


static OTF2_CallbackCode
cart_topology_forward_cb( void*                        userData,
                          OTF2_CartTopologyRef         self,
                          OTF2_StringRef               name,
                          OTF2_CommRef                 communicator,
                          uint8_t                      numberOfDimensions,
                          const OTF2_CartDimensionRef* cartDimensions )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCartTopology( self,
                                          name,
                                          communicator,
                                          numberOfDimensions,
                                          cartDimensions );
}


static OTF2_CallbackCode
cart_coordinate_forward_cb( void*                userData,
                            OTF2_CartTopologyRef cartTopology,
                            uint32_t             rank,
                            uint8_t              numberOfDimensions,
                            const uint32_t*      coordinates )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCartCoordinate( cartTopology,
                                            rank,
                                            numberOfDimensions,
                                            coordinates );
}


static OTF2_CallbackCode
source_code_location_forward_cb( void*                      userData,
                                 OTF2_SourceCodeLocationRef self,
                                 OTF2_StringRef             file,
                                 uint32_t                   lineNumber )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSourceCodeLocation( self,
                                                file,
                                                lineNumber );
}


static OTF2_CallbackCode
calling_context_forward_cb( void*                      userData,
                            OTF2_CallingContextRef     self,
                            OTF2_RegionRef             region,
                            OTF2_SourceCodeLocationRef sourceCodeLocation,
                            OTF2_CallingContextRef     parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallingContext( self,
                                            region,
                                            sourceCodeLocation,
                                            parent );
}


static OTF2_CallbackCode
calling_context_property_forward_cb( void*                  userData,
                                     OTF2_CallingContextRef callingContext,
                                     OTF2_StringRef         name,
                                     OTF2_Type              type,
                                     OTF2_AttributeValue    value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallingContextProperty( callingContext,
                                                    name,
                                                    type,
                                                    value );
}


static OTF2_CallbackCode
interrupt_generator_forward_cb( void*                       userData,
                                OTF2_InterruptGeneratorRef  self,
                                OTF2_StringRef              name,
                                OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                OTF2_Base                   base,
                                int64_t                     exponent,
                                uint64_t                    period )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefInterruptGenerator( self,
                                                name,
                                                interruptGeneratorMode,
                                                base,
                                                exponent,
                                                period );
}
} // extern "C"

Otf2Definitions::Otf2Definitions( OTF2_Reader* reader ) :
    m_Reader( reader )
{
    OTF2_GlobalDefReader* def_reader = OTF2_Reader_GetGlobalDefReader( reader );

    OTF2_GlobalDefReaderCallbacks* def_callbacks = OTF2_GlobalDefReaderCallbacks_New();

    OTF2_GlobalDefReaderCallbacks_SetClockPropertiesCallback( def_callbacks, clock_properties_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetParadigmCallback( def_callbacks, paradigm_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetParadigmPropertyCallback( def_callbacks, paradigm_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetStringCallback( def_callbacks, string_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetAttributeCallback( def_callbacks, attribute_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeCallback( def_callbacks, system_tree_node_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback( def_callbacks, location_group_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationCallback( def_callbacks, location_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetRegionCallback( def_callbacks, region_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallsiteCallback( def_callbacks, callsite_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallpathCallback( def_callbacks, callpath_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetGroupCallback( def_callbacks, group_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricMemberCallback( def_callbacks, metric_member_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricClassCallback( def_callbacks, metric_class_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricInstanceCallback( def_callbacks, metric_instance_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCommCallback( def_callbacks, comm_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetParameterCallback( def_callbacks, parameter_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetRmaWinCallback( def_callbacks, rma_win_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricClassRecorderCallback( def_callbacks, metric_class_recorder_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodePropertyCallback( def_callbacks, system_tree_node_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeDomainCallback( def_callbacks, system_tree_node_domain_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupPropertyCallback( def_callbacks, location_group_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationPropertyCallback( def_callbacks, location_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCartDimensionCallback( def_callbacks, cart_dimension_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCartTopologyCallback( def_callbacks, cart_topology_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCartCoordinateCallback( def_callbacks, cart_coordinate_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSourceCodeLocationCallback( def_callbacks, source_code_location_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallingContextCallback( def_callbacks, calling_context_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallingContextPropertyCallback( def_callbacks, calling_context_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetInterruptGeneratorCallback( def_callbacks, interrupt_generator_forward_cb );

    OTF2_Reader_RegisterGlobalDefCallbacks( reader, def_reader, def_callbacks, this );

    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllGlobalDefinitions( reader, def_reader, &definitions_read );

    OTF2_GlobalDefReaderCallbacks_Delete( def_callbacks );

    OTF2_Reader_CloseGlobalDefReader( reader,
                                      def_reader );
}

Otf2Definitions::~Otf2Definitions()
{
    for ( ClockPropertiesIT it = m_ClockPropertiess.begin();
          it != m_ClockPropertiess.end();
          it++ )
    {
        delete *it;
    }

    for ( ParadigmIT it = m_Paradigms.begin();
          it != m_Paradigms.end();
          it++ )
    {
        delete *it;
    }

    for ( ParadigmPropertyIT it = m_ParadigmPropertys.begin();
          it != m_ParadigmPropertys.end();
          it++ )
    {
        delete *it;
    }


    for ( AttributeIT it = m_Attributes.begin();
          it != m_Attributes.end();
          it++ )
    {
        delete it->second;
    }

    for ( SystemTreeNodeIT it = m_SystemTreeNodes.begin();
          it != m_SystemTreeNodes.end();
          it++ )
    {
        for ( SystemTreeNodePropertyIT sit = it->second->systemTreeNodePropertys.begin();
              sit != it->second->systemTreeNodePropertys.end();
              sit++ )
        {
            delete *sit;
        }
        for ( SystemTreeNodeDomainIT sit = it->second->systemTreeNodeDomains.begin();
              sit != it->second->systemTreeNodeDomains.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( LocationGroupIT it = m_LocationGroups.begin();
          it != m_LocationGroups.end();
          it++ )
    {
        for ( LocationGroupPropertyIT sit = it->second->locationGroupPropertys.begin();
              sit != it->second->locationGroupPropertys.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( LocationIT it = m_Locations.begin();
          it != m_Locations.end();
          it++ )
    {
        for ( LocationPropertyIT sit = it->second->locationPropertys.begin();
              sit != it->second->locationPropertys.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( RegionIT it = m_Regions.begin();
          it != m_Regions.end();
          it++ )
    {
        delete it->second;
    }

    for ( CallsiteIT it = m_Callsites.begin();
          it != m_Callsites.end();
          it++ )
    {
        delete it->second;
    }

    for ( CallpathIT it = m_Callpaths.begin();
          it != m_Callpaths.end();
          it++ )
    {
        delete it->second;
    }

    for ( GroupIT it = m_Groups.begin();
          it != m_Groups.end();
          it++ )
    {
        delete it->second;
    }

    for ( MetricMemberIT it = m_MetricMembers.begin();
          it != m_MetricMembers.end();
          it++ )
    {
        delete it->second;
    }

    for ( CommIT it = m_Comms.begin();
          it != m_Comms.end();
          it++ )
    {
        delete it->second;
    }

    for ( ParameterIT it = m_Parameters.begin();
          it != m_Parameters.end();
          it++ )
    {
        delete it->second;
    }

    for ( RmaWinIT it = m_RmaWins.begin();
          it != m_RmaWins.end();
          it++ )
    {
        delete it->second;
    }

    for ( CartDimensionIT it = m_CartDimensions.begin();
          it != m_CartDimensions.end();
          it++ )
    {
        delete it->second;
    }

    for ( CartTopologyIT it = m_CartTopologys.begin();
          it != m_CartTopologys.end();
          it++ )
    {
        for ( CartCoordinateIT sit = it->second->cartCoordinates.begin();
              sit != it->second->cartCoordinates.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( SourceCodeLocationIT it = m_SourceCodeLocations.begin();
          it != m_SourceCodeLocations.end();
          it++ )
    {
        delete it->second;
    }

    for ( CallingContextIT it = m_CallingContexts.begin();
          it != m_CallingContexts.end();
          it++ )
    {
        for ( CallingContextPropertyIT sit = it->second->callingContextPropertys.begin();
              sit != it->second->callingContextPropertys.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( InterruptGeneratorIT it = m_InterruptGenerators.begin();
          it != m_InterruptGenerators.end();
          it++ )
    {
        delete it->second;
    }


    for ( MetricIT it = m_Metrics.begin();
          it != m_Metrics.end();
          it++ )
    {
        Metric* super = it->second;
        switch ( super->metricType )
        {
            case METRIC_CLASS:
                delete reinterpret_cast<MetricClass*>( super );
                break;

            case METRIC_INSTANCE:
                delete reinterpret_cast<MetricInstance*>( super );
                break;
        }
    }
}
